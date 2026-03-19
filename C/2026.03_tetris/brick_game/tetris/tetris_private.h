#ifndef TETRIS_PRIVATE_H
#define TETRIS_PRIVATE_H

#include <stdint.h>  //uint16_t

typedef struct GameInfo_t GameInfo_t;

#define ROTATIONS_COUNT 4

uint16_t GetFigure(int figure_type, int rotation);
int** CreateZeroField(const int width, const int height);
void MoveFigureHorizontal(int** field, const int dx);
void UseOffset(int* fig_x, int* fig_y, int type, int rot);
void DropFigure(GameInfo_t* game);
void LockAndSpawn(GameInfo_t* game);
int GetNextFigureType(int** next);
void AddScore(GameInfo_t* game, const int lines);
int RemoveFullLines(int** field);
int CanMoveTo(int** field, int fig_x, int fig_y, int figure_type, int rotation);
void CheckResultForCanMoveTo(int** field, int new_x, int new_y,
                             int* for_return);
void RotateFigure(int** field);
void ClearFigureFromField(int** field, int fig_x, int fig_y);
void PlaceFigureOnField(int** field, int fig_x, int fig_y, int figure_type,
                        int rotation);
void PlaceNextFigure(int** next_field, int figure_type);
void ClearNext(int** next);
int GetRandomFigure(void);
void FindFigureOnField(int** field, int* fig_x, int* fig_y, int* type,
                       int* rot);
void GetFigureOffset(int figure_type, int rotation, int* dx, int* dy);
void MoveDown(GameInfo_t* game);
long CurrentTimeMs();
void DecideInputDownWitchoutRepeat(GameInfo_t* game, long* last_down_time);
void MoveAllTopLinesDown(int** field, int y);
void SafelyWriteValueToCell(int** field, int fig_x, int fig_y, const int row,
                            const int col, const int value);
#endif