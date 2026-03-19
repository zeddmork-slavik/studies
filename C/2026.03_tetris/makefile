SHELL = /bin/sh
COMMON_FLAGS := gcc -std=c11 -Wall -Werror -Wextra
INSTALL_DIR := $(HOME)/.local/bin
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    CHECK_LIBS := -lcheck -lsubunit -lm
    NCURSES := -lncursesw
else
    CHECK_LIBS := -lcheck  -lm
    NCURSES := -lncurses
endif

SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_mixer 2>/dev/null)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_mixer 2>/dev/null)

SRC_SIMPLE := gui/cli/main.c gui/cli/cli.c brick_game/tetris/tetris.c \
brick_game/tetris/tetris2.c
SRC_WITH_AUDIO := gui/cli/main2.c gui/cli/cli2.c gui/cli/audio.c \
brick_game/tetris/tetris.c brick_game/tetris/tetris2.c
TARGET := tetris

all: $(TARGET)

$(TARGET): $(SRC_SIMPLE)
	$(COMMON_FLAGS) $(SRC_SIMPLE) -o $(TARGET) $(NCURSES)

gcov_report: test
	mkdir -p coverage
	gcovr --html-details coverage/report.html --root . --exclude tests/
	@open coverage/report.html 2>/dev/null || \
	xdg-open coverage/report.html 2>/dev/null || \
	echo "Open coverage/report.html manually"

clean:
	rm -rf coverage/
	rm -f *.gcda *.gcno
	rm -f test_tetris
	rm -f $(TARGET)
	rm -f disco

test: tests/t.c brick_game/tetris/tetris.c brick_game/tetris/tetris2.c
	$(COMMON_FLAGS) --coverage $^ -o test_tetris $(CHECK_LIBS)
	./test_tetris

disco: $(SRC_WITH_AUDIO)
	$(COMMON_FLAGS) $(SRC_WITH_AUDIO) -o disco $(NCURSES) \
	$(SDL_FLAGS) $(SDL_LIBS)

.PHONY: all test clean gcov_report disco
