#ifndef CLI_H
#define CLI_H

#include <ncurses.h>

typedef struct GameInfo_t GameInfo_t;
typedef enum UserAction_t UserAction_t;

#define NCURSE_INIT(time)     \
    {                         \
        initscr();            \
        noecho();             \
        curs_set(0);          \
        keypad(stdscr, TRUE); \
        timeout(time);        \
    }

#define MVADDCH(y, x, c) \
    mvaddch(BOARDS_BEGIN + (y), BOARDS_BEGIN + (x), c)  // обёртка под нкурс функцию для добавки BOARDS_BEGIN
#define BOARDS_BEGIN 2                                  // отступ экрана от края терминала
#define BOARD_HEIGHT 20
#define BOARD_WIDTH 20

void print_frame_for_playing_field(int top_y, int bottom_y, int left_x, int right_x);
void draw_frame(const GameInfo_t* game);
void draw_field(int** field);
void draw_next(int** next);
void draw_stats(const GameInfo_t* game);
void draw_welcome_screen(int ch);
UserAction_t parse_action(const int ch);
void draw_end(const GameInfo_t* game);
#endif