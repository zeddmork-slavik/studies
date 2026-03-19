#include "../../brick_game/tetris/tetris.h"
#include "cli.h"

void draw_welcome_screen(int ch) {
    clear();

    int center_y = LINES / 2 - 5;
    int center_x = COLS / 2 - 20;

    mvprintw(center_y, center_x, "╔══════════════════════════════════╗");
    mvprintw(center_y + 1, center_x, "║           TETRIS GAME            ║");
    mvprintw(center_y + 2, center_x, "╠══════════════════════════════════╣");
    mvprintw(center_y + 3, center_x, "║ Управление:                      ║");
    mvprintw(center_y + 4, center_x, "║ ← → - движение                   ║");
    mvprintw(center_y + 5, center_x, "║ space - поворот                  ║");
    mvprintw(center_y + 6, center_x, "║ ↓   - сбросить фигуру вниз       ║");
    mvprintw(center_y + 7, center_x, "║                                  ║");
    mvprintw(center_y + 8, center_x, "║ P    - пауза                     ║");
    mvprintw(center_y + 9, center_x, "║ Q    - выход                     ║");
    mvprintw(center_y + 10, center_x, "╠══════════════════════════════════╣");
    mvprintw(center_y + 11, center_x, "║ Нажмите ENTER для начала игры    ║");
    mvprintw(center_y + 12, center_x, "╚══════════════════════════════════╝");

    refresh();

    do {
        ch = getch();
    } while (ch != '\n');
}

void print_frame_for_playing_field(int top_y, int bottom_y, int left_x, int right_x) {
    mvaddch(top_y, left_x, ACS_ULCORNER);
    int cursor_for_topline = left_x + 1;
    for (; cursor_for_topline <= right_x; cursor_for_topline++) {
        mvaddch(top_y, cursor_for_topline, ACS_HLINE);
    }
    mvaddch(top_y, cursor_for_topline, ACS_URCORNER);

    for (int cursor_by_hight = top_y + 1; cursor_by_hight <= bottom_y; cursor_by_hight++) {
        mvaddch(cursor_by_hight, left_x, ACS_VLINE);
        mvaddch(cursor_by_hight, right_x + 1, ACS_VLINE);
    }
    mvaddch(bottom_y + 1, left_x, ACS_LLCORNER);

    int cursor_for_bottomline = left_x + 1;
    for (; cursor_for_bottomline <= right_x; cursor_for_bottomline++) {
        mvaddch(bottom_y + 1, cursor_for_bottomline, ACS_HLINE);
    }
    mvaddch(bottom_y + 1, cursor_for_bottomline, ACS_LRCORNER);
}

UserAction_t parse_action(const int ch) {
    UserAction_t action;

    switch (ch) {
        case KEY_LEFT:
            action = Left;
            break;
        case KEY_RIGHT:
            action = Right;
            break;
        case KEY_DOWN:
            action = Down;
            break;
        case ' ':
            action = Action;
            break;
        case 'p':
        case 'P':
            action = Pause;
            break;
        case 'q':
        case 'Q':
            action = Terminate;
            break;
        case '\n':
            action = Start;
            break;
        default:
            action = -1;
            break;
    }
    return action;
}

void decide_visible_or_not(int* flash_visible) {
    static int flash_counter = 0;
    flash_counter++;
    static int last_switch_frame = 0;
    if (flash_counter < 450) {
        if (flash_counter - last_switch_frame >= 10) {  // каждые 10 кадров переключаем
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 930) {
        if (flash_counter - last_switch_frame >= 15) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 1470) {
        if (flash_counter - last_switch_frame >= 6) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 1860) {
        if (flash_counter - last_switch_frame >= 20) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 2810) {
        if (flash_counter - last_switch_frame >= 6) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 3670) {
        if (flash_counter - last_switch_frame >= 20) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else if (flash_counter < 5100) {
        if (flash_counter - last_switch_frame >= 6) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    } else {
        if (flash_counter - last_switch_frame >= 20) {
            *flash_visible = !*flash_visible;
            last_switch_frame = flash_counter;
        }
    }
}

void draw_frame(const GameInfo_t* game) {
    static int flash_visible = 1;  // 1 — поле видно, 0 — только чёрный экран
    decide_visible_or_not(&flash_visible);

    if (flash_visible) {
        print_frame_for_playing_field(0, BOARD_HEIGHT, 0, BOARD_WIDTH);
        draw_field(game->field);
        draw_next(game->next);
        draw_stats(game);
    } else {
        clear();
    }
}

void draw_field(int** field) {
    for (int row = 0; row < FIELD_HEIGHT; row++) {
        for (int col = 0; col < FIELD_WIDTH; col++) {
            int term_y = 1 + row;
            int term_x = 1 + col * 2;
            int cell = field[row][col];  // 0 = пусто, 1..7 = тип фигуры

            if (cell > 0) {
                cell = cell % 10;          // в cell ещё поворот десятками зашит
                attron(COLOR_PAIR(cell));  // и пары цветов и значение индекса начинаются с единиц
                mvaddstr(term_y, term_x, "██");
                attroff(COLOR_PAIR(cell));
            } else {
                mvaddstr(term_y, term_x, "  ");
            }
        }
    }
}

void draw_next(int** next) {
    int start_y = 3;
    int start_x = FIELD_WIDTH * 2 + 5;

    mvprintw(start_y, start_x, "NEXT:");

    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int val = next[r][c];
            if (val > 0) {
                attron(COLOR_PAIR(val));
                mvaddstr(start_y + r, start_x + 6 + c * 2, "██");
                attroff(COLOR_PAIR(val));
            } else {
                mvaddstr(start_y + r, start_x + 6 + c * 2, "  ");
            }
        }
    }
}

void draw_stats(const GameInfo_t* game) {
    int start_y = 6;
    int start_x = FIELD_WIDTH * 2 + 5;
    if (game->pause) {
        mvprintw(start_y, start_x, "PAUSE ON");
    } else {
        mvprintw(start_y, start_x,
                 "        ");  // иначе "PAUSE" остаётся после выключения
    }
    mvprintw(++start_y, start_x, "SCORE: %d", game->score);
    start_y += 4;
    mvprintw(start_y, start_x, "HIGH SCORE: %d", game->high_score);
    start_y += 4;
    mvprintw(start_y, start_x, "LEVEL: %d", game->level);
    start_y += 4;
    mvprintw(start_y, start_x, "SPEED: %d", game->speed);
    mvprintw(++start_y, start_x, "P - PAUSE");
    mvprintw(++start_y, start_x, "Q - EXIT");
}

void draw_end(const GameInfo_t* game) {
    clear();
    int center_y = LINES / 2 - 2;
    int center_x = COLS / 2 - 10;

    attron(A_BOLD);
    mvprintw(center_y, center_x, "╔══════════════╗");
    mvprintw(center_y + 1, center_x, "║  GAME OVER   ║");
    mvprintw(center_y + 2, center_x, "╚══════════════╝");
    attroff(A_BOLD);

    mvprintw(center_y + 4, center_x - 5, "Press ENTER to exit");
    mvprintw(center_y + 5, center_x, "YOUR SCORE: %d", game->score);
    mvprintw(center_y + 6, center_x, "HIGH SCORE: %d", game->high_score);
    if (game->score > game->high_score) {
        mvprintw(center_y + 7, center_x - 5, "Congratulations, you have set a new record");
    }
}