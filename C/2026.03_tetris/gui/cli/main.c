#include <locale.h>

#include "../../brick_game/tetris/tetris.h"
#include "cli.h"

void init_figure_colors(void) {
    start_color();         // Включаем поддержку цветов
    use_default_colors();  // Разрешаем прозрачный фон (-1)

    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_BLUE, -1);
    init_pair(4, COLOR_MAGENTA, -1);
    init_pair(5, COLOR_GREEN, -1);
    init_pair(6, COLOR_CYAN, -1);
    init_pair(7, COLOR_YELLOW, -1);
}

int main(void) {
    setlocale(LC_ALL, "");  // UTF-8 поддержка
    NCURSE_INIT(-1);        // для экрана приветствия
    init_figure_colors();
    int ch = 0;
    draw_welcome_screen(ch);
    timeout(33);  // 30 FPS
    clear();
    GameInfo_t game;
    int running = InitGame(&game);
    while (running) {
        ch = getch();
        if (ch != ERR) {
            UserAction_t action = parse_action(ch);
            if (action == Terminate) {
                running = 0;
            } else if ((int)action != -1) {
                RealUserInput(action, &game);
            }
        }
        RealUpdateCurrentState(&game);
        if (CheckGameOver(&game, &running)) {
            timeout(-1);
            draw_end(&game);
            refresh();
            getch();
        } else {
            draw_frame(&game);
            refresh();
        }
    }
    FreeGame(&game);
    endwin();
    return 0;
}
