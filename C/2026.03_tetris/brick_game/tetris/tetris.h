#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define FIGURES_COUNT 7

typedef enum UserAction_t {
    Start,
    Pause,
    Terminate,  // выход
    Left,
    Right,
    Up,  // не используется
    Down,
    Action
} UserAction_t;

typedef struct GameInfo_t {
    int** field;
    int** next;  // динамический массив 4x4 для следующей фигуры
    int score;
    int high_score;
    int level;
    int speed;
    int pause;
} GameInfo_t;

void RealUserInput(const UserAction_t action, GameInfo_t* game);
void RealUpdateCurrentState(GameInfo_t* game);
int InitGame(GameInfo_t* game);
int CheckGameOver(const GameInfo_t* game, int* running);
void FreeGame(GameInfo_t* game);

#endif