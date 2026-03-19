#include <stdint.h>    //uint16_t
#include <string.h>    // memcpy в place_next_figure()
#include <sys/time.h>  //for gettimeofday

#include "tetris.h"

uint16_t GetFigure(int figure_type, int rotation) {
    // вращение направо
    static const uint16_t ALL_PIECES[FIGURES_COUNT][4] = {
        // палочка 0
        {0xF000, 0x2222, 0xF000, 0x4444},
        // квадратик 1
        {0x6600, 0x6600, 0x6600, 0x6600},  // проще лишние разы присвоить то же значение чем при поворотах
        // всех фигур проверять квадрат ли это
        // треугольник 2
        {0xE400, 0x4C40, 0x4E00, 0x4640},
        // Z 3
        {0xC600, 0x2640, 0x6300, 0x2640},
        // S (4)
        {0x6C00, 0x8C40, 0x6C00, 0x4620},
        // J (5)
        {0x4700, 0x6440, 0x7100, 0x2260},
        // L (6)
        {0x2E00, 0x4460, 0x7400, 0x6220}};

    return ALL_PIECES[figure_type][rotation];
}

void ClearNext(int** next) {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++) {
            next[y][x] = 0;
        }
}

void PlaceNextFigure(int** next, int figure_type) {
    static const int NEXT_MATRICES[FIGURES_COUNT][4][4] = {
        // I (0)
        {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        // O (1)
        {{0, 0, 0, 0}, {0, 2, 2, 0}, {0, 2, 2, 0}, {0, 0, 0, 0}},
        // T (2)
        {{3, 3, 3, 0}, {0, 3, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        // Z (3)
        {{4, 4, 0, 0}, {0, 4, 4, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        // S (4)
        {{0, 5, 5, 0}, {5, 5, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        // J (5)
        {{0, 6, 0, 0}, {0, 6, 6, 6}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        // L (6)
        {{0, 0, 7, 0}, {7, 7, 7, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
    // циферки разные потому что туда сразу тип фигуры кладу
    ClearNext(next);

    memcpy(next[0], NEXT_MATRICES[figure_type], 16 * sizeof(int));
}

void GetFigureOffset(int figure_type, int rotation, int* dx, int* dy) {
    static const int OFFSETS[FIGURES_COUNT][4][2] = {// 0: I (палочка)
                                                     {{0, 0}, {2, 0}, {0, 0}, {1, 0}},
                                                     // 1: O (квадрат)
                                                     {{1, 0}, {1, 0}, {1, 0}, {1, 0}},
                                                     // 2: T (треугольник)
                                                     {{0, 0}, {1, 0}, {1, 0}, {1, 0}},
                                                     // 3: Z
                                                     {{0, 0}, {2, 0}, {1, 0}, {2, 0}},
                                                     // 4: S
                                                     {{1, 0}, {0, 0}, {1, 0}, {1, 0}},
                                                     // 5: J
                                                     {{1, 0}, {1, 0}, {1, 0}, {2, 0}},
                                                     // 6: L
                                                     {{2, 0}, {1, 0}, {1, 0}, {1, 0}}};

    *dx = OFFSETS[figure_type][rotation][0];  // небольшая путаница что первый индексом идёт столбец x -
                                              // но это только здесь))
    *dy = OFFSETS[figure_type][rotation][1];
}

void UseOffset(int* fig_x, int* fig_y, int type, int rot) {
    int offset_x, offset_y;
    GetFigureOffset(type, rot, &offset_x, &offset_y);
    *fig_x -= offset_x;
    *fig_y -= offset_y;
}

long CurrentTimeMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void MoveAllTopLinesDown(int** field, int y) {
    for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[yy - 1][x]) {
                field[yy][x] = field[yy - 1][x];
            } else {
                field[yy][x] = 0;
            }
        }
    }
    // Верхнюю строку очищаем
    for (int x = 0; x < FIELD_WIDTH; x++) {
        field[0][x] = 0;
    }
}

int RemoveFullLines(int** field) {
    int lines_removed = 0;
    for (int y = FIELD_HEIGHT - 1; y >= 0;) {  // идём снизу вверх
        int full = 1;
        for (int x = 0, should_continue = 1; x < FIELD_WIDTH && should_continue; x++) {
            if (!field[y][x]) {
                full = 0;
                should_continue = 0;
            }
        }

        if (full) {
            MoveAllTopLinesDown(field, y);
            lines_removed++;
        } else {
            y--;  // если строка была полной мы её удалили, вышестоящую спустили =
                  // проверяем этот же индекс но там строка что была выше
        }
    }
    return lines_removed;
}

void SafelyWriteValueToCell(int** field, int fig_x, int fig_y, const int row, const int col,
                            const int value) {
    fig_y += row;
    fig_x += col;
    if (fig_y >= 0 && fig_y < FIELD_HEIGHT && fig_x >= 0 && fig_x < FIELD_WIDTH) {
        field[fig_y][fig_x] = value;
    }
}

void PlaceFigureOnField(int** field, int fig_x, int fig_y, int figure_type, int rotation) {
    uint16_t figure = GetFigure(figure_type, rotation);
    int value = rotation * 10 + (figure_type + 1);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int bit = (figure >> (15 - (row * 4 + col))) & 1;  // 15 - (row * 4 + col) — инверсия индекса бита
            if (bit) {
                SafelyWriteValueToCell(field, fig_x, fig_y, row, col, value);
            }
        }
    }
}

void ClearFigureFromField(int** field, int fig_x, int fig_y) {
    for (int dy = 0; dy < 4; dy++) {
        for (int dx = 0; dx < 4; dx++) {
            int y = fig_y + dy;
            int x = fig_x + dx;
            if (y < FIELD_HEIGHT && x < FIELD_WIDTH && field[y][x] < 100) {
                field[y][x] = 0;
            }
        }
    }
}

void FindFigureOnField(int** field, int* fig_x, int* fig_y, int* type, int* rot) {
    for (int y = 0; y < FIELD_HEIGHT && *fig_x == -1; y++) {
        for (int x = 0; x < FIELD_WIDTH && *fig_y == -1; x++) {
            int val = field[y][x];
            if (val > 0 && val < 100) {
                *type = (val % 10) - 1;
                *rot = val / 10;
                *fig_x = x;
                *fig_y = y;
            }
        }
    }
}

void CheckResultForCanMoveTo(int** field, int new_x, int new_y, int* for_return) {
    if (new_x < 0 || new_x >= FIELD_WIDTH || new_y < 0 || new_y >= FIELD_HEIGHT) {
        *for_return = 0;
    } else if (field[new_y][new_x] >= 100) {
        *for_return = 0;
    }
}

int CanMoveTo(int** field, int fig_x, int fig_y, int figure_type, int rotation) {
    int for_return = 1;
    uint16_t figure = GetFigure(figure_type, rotation);

    for (int row = 0; row < 4 && for_return; row++) {
        for (int col = 0; col < 4 && for_return; col++) {
            int bit = (figure >> (15 - (row * 4 + col))) & 1;
            if (bit) {
                int new_x = fig_x + col;
                int new_y = fig_y + row;
                CheckResultForCanMoveTo(field, new_x, new_y, &for_return);
            }
        }
    }
    return for_return;
}
