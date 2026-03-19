#include "tetris.h"

#include <stdio.h>
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time for srand()

#include "tetris_private.h"

void RealUpdateCurrentState(GameInfo_t* game) {
    static const int frame_delays[11] = {0, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3};
    if (!game->pause) {
        static int frame_counter = 0;
        frame_counter++;

        if (frame_counter >= frame_delays[game->level]) {
            frame_counter = 0;
            MoveDown(game);
        }
    }
}

int** CreateZeroField(const int width, const int height) {
    int** field = NULL;
    int* data = NULL;
    int success = 0;

    field = calloc(height, sizeof(int*));
    if (field) {
        data = calloc(height * width, sizeof(int));
        if (data) {
            for (int i = 0; i < height; i++) {
                field[i] = data + i * width;
            }
            success = 1;
        }
    }

    if (!success) {
        if (data) free(data);
        if (field) free(field);
        field = NULL;
    }

    return field;
}

int InitGame(GameInfo_t* game) {
    game->field = CreateZeroField(FIELD_WIDTH, FIELD_HEIGHT);
    game->next = CreateZeroField(4, 4);
    int ok = game->field && game->next;
    if (ok) {
        game->score = 0;

        FILE* file = fopen("highscore.dat", "rb");
        if (file) {
            fread(&game->high_score, sizeof(int), 1, file);
            fclose(file);
        } else {
            game->high_score = 0;
        }

        game->level = 1;
        game->speed = 1;
        game->pause = 0;
        srand(time(NULL));
        PlaceNextFigure(game->next, GetRandomFigure());
        PlaceFigureOnField(game->field, 3, 0, GetRandomFigure(), 0);
    }
    return ok;
}

int GetRandomFigure() { return (rand() % 7); }

void RealUserInput(const UserAction_t action, GameInfo_t* game) {
    static long last_down_time = 0;
    if (game->pause) {
        switch (action) {
            case Terminate:
                break;
            case Pause:
                game->pause = !game->pause;
                break;
            default:
                return;
        }
    } else {
        switch (action) {
            case Left:
                MoveFigureHorizontal(game->field, -1);
                break;
            case Right:
                MoveFigureHorizontal(game->field, 1);
                break;
            case Down:
                DecideInputDownWitchoutRepeat(game, &last_down_time);
                break;
            case Action:
                RotateFigure(game->field);
                break;
            case Pause:
                game->pause = !game->pause;
                break;
            default:  // чтоб компилятор не ругался на отсутсвие других
                break;
        }
    }
}

void DecideInputDownWitchoutRepeat(GameInfo_t* game, long* last_down_time) {
    long now = CurrentTimeMs();
    if (now - *last_down_time > 200) {
        DropFigure(game);
        *last_down_time = now;
    }
}

void DropFigure(GameInfo_t* game) {
    int fig_x = -1, fig_y = -1, type, rot;
    FindFigureOnField(game->field, &fig_x, &fig_y, &type, &rot);
    UseOffset(&fig_x, &fig_y, type, rot);
    int new_y = fig_y;
    while (CanMoveTo(game->field, fig_x, new_y + 1, type, rot)) {
        new_y++;
    }
    if (new_y != fig_y) {
        ClearFigureFromField(game->field, fig_x, fig_y);
        PlaceFigureOnField(game->field, fig_x, new_y, type, rot);
        LockAndSpawn(game);
    }
}

void LockAndSpawn(GameInfo_t* game) {
    int** field = game->field;  // говорят так компилятору быстрее каждый раз гейм
                                // не разыменовывать
    // Превращаем текущую фигуру в постройку
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] > 0 && field[y][x] < 100) {
                field[y][x] = 102;
            }
        }
    }

    int lines = RemoveFullLines(game->field);
    if (lines) {
        AddScore(game, lines);
    }

    int next_type = GetNextFigureType(game->next);
    PlaceFigureOnField(field, 3, 0, next_type, 0);
    PlaceNextFigure(game->next, GetRandomFigure());
}

int GetNextFigureType(int** next) {
    int for_return = -1;
    for (int y = 0; y < 4 && for_return == -1; y++) {
        for (int x = 0; x < 4 && for_return == -1; x++) {
            if (next[y][x] > 0) {
                for_return = next[y][x] - 1;
            }
        }
    }
    return for_return;
}

void AddScore(GameInfo_t* game, const int lines) {
    switch (lines) {
        case 1:
            game->score += 100;
            break;
        case 2:
            game->score += 300;
            break;
        case 3:
            game->score += 700;
            break;
        case 4:
            game->score += 1500;
    }

    static bool dont_check_score = 0;
    if (!dont_check_score) {
        game->level = game->score / 600 + 1;
        if (game->level >= 10) {
            game->level = 10;
            dont_check_score = true;
        }
        game->speed = game->level;
    }

    if (game->score > game->high_score) {
        FILE* file = fopen("highscore.dat", "wb");
        if (file) {
            fwrite(&game->score, sizeof(int), 1, file);
            fclose(file);
        }
    }
}

void MoveFigureHorizontal(int** field, const int dx) {
    int fig_x = -1, fig_y = -1, type, rot;
    FindFigureOnField(field, &fig_x, &fig_y, &type, &rot);
    UseOffset(&fig_x, &fig_y, type, rot);

    if (CanMoveTo(field, fig_x + dx, fig_y, type, rot)) {
        ClearFigureFromField(field, fig_x, fig_y);
        PlaceFigureOnField(field, fig_x + dx, fig_y, type, rot);
    }
}

void RotateFigure(int** field) {
    int fig_x = -1, fig_y = -1, type, rot;
    FindFigureOnField(field, &fig_x, &fig_y, &type, &rot);
    UseOffset(&fig_x, &fig_y, type, rot);
    rot = (rot + 1) % 4;
    int success = 0;
    const int shifts[] = {0, -1, 1, -2, 2};
    int new_x = 0;
    for (int i = 0; i < 5 && !success; i++) {
        new_x = fig_x + shifts[i];
        if (CanMoveTo(field, new_x, fig_y, type, rot)) {
            success = 1;
        }
    }

    if (success) {
        ClearFigureFromField(field, fig_x, fig_y);
        PlaceFigureOnField(field, new_x, fig_y, type, rot);
    }
}

void MoveDown(GameInfo_t* game) {
    int fig_x = -1, fig_y = -1, type, rot;
    FindFigureOnField(game->field, &fig_x, &fig_y, &type, &rot);
    UseOffset(&fig_x, &fig_y, type, rot);
    if (CanMoveTo(game->field, fig_x, fig_y + 1, type, rot)) {
        ClearFigureFromField(game->field, fig_x, fig_y);
        PlaceFigureOnField(game->field, fig_x, fig_y + 1, type, rot);
    } else {
        LockAndSpawn(game);
    }
}

int CheckGameOver(const GameInfo_t* game, int* running) {
    for (int x = 0, should_continue = 1; x < FIELD_WIDTH && should_continue; x++) {
        if (game->field[0][x] >= 100) {
            *running = 0;
            should_continue = 0;
        }
    }
    if (*running) {
        int next_type = GetNextFigureType(game->next);
        if (!CanMoveTo(game->field, 3, 0, next_type, 0)) {
            *running = 0;
        }
    }

    return !(*running);
}

void FreeGame(GameInfo_t* game) {
    if (game->field) {
        free(game->field[0]);
        free(game->field);
    }

    if (game->next) {
        free(game->next[0]);
        free(game->next);
    }
}