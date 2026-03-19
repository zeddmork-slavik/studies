#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../brick_game/tetris/tetris.h"
#include "../brick_game/tetris/tetris_private.h"

START_TEST(test_init) {
  GameInfo_t game;
  int result = InitGame(&game);

  ck_assert_int_eq(result, 1);
  ck_assert_int_eq(game.score, 0);
  ck_assert_int_eq(game.level, 1);
  ck_assert_int_eq(game.pause, 0);

  FreeGame(&game);
}
END_TEST

START_TEST(test_left) {
  GameInfo_t game;
  InitGame(&game);

  int before = game.field[0][3];  // запоминаем первую клетку фигуры
  RealUserInput(Left, &game);
  int after = game.field[0][2];  // должна сместиться влево

  ck_assert_int_eq(before, after);

  FreeGame(&game);
}
END_TEST

START_TEST(test_right) {
  GameInfo_t game;
  InitGame(&game);

  int before = game.field[0][3];
  RealUserInput(Right, &game);
  int after = game.field[0][4];

  ck_assert_int_eq(before, after);

  FreeGame(&game);
}
END_TEST

START_TEST(test_rotate) {
  GameInfo_t game;

  // Вручную создаём поле
  game.field = CreateZeroField(FIELD_WIDTH, FIELD_HEIGHT);
  game.next = CreateZeroField(4, 4);
  game.score = 0;
  game.level = 1;
  game.pause = 0;

  // Ставим I-фигуру (палочку) горизонтально вручную
  // Значение: rotation 0 (0*10) + тип 0+1 = 1
  game.field[0][3] = 1;
  game.field[0][4] = 1;
  game.field[0][5] = 1;
  game.field[0][6] = 1;

  // Поворачиваем
  RotateFigure(game.field);

  // Проверяем, что стала вертикально
  ck_assert_int_eq(game.field[0][4], 0);   // старая позиция
  ck_assert_int_eq(game.field[1][5], 11);  // новая позиция
  ck_assert_int_eq(game.field[2][5], 11);
  ck_assert_int_eq(game.field[3][5], 11);

  FreeGame(&game);
}
END_TEST

START_TEST(test_drop) {
  GameInfo_t game;
  InitGame(&game);

  RealUserInput(Down, &game);

  // Проверяем, что внизу появилась постройка
  int has_building = 0;
  for (int x = 0; x < FIELD_WIDTH; x++) {
    if (game.field[FIELD_HEIGHT - 1][x] >= 100) {
      has_building = 1;
    }
  }

  ck_assert_int_eq(has_building, 1);

  FreeGame(&game);
}
END_TEST

START_TEST(test_pause) {
  GameInfo_t game;
  InitGame(&game);

  ck_assert_int_eq(game.pause, 0);

  RealUserInput(Pause, &game);
  ck_assert_int_eq(game.pause, 1);

  RealUserInput(Pause, &game);
  ck_assert_int_eq(game.pause, 0);

  FreeGame(&game);
}
END_TEST

START_TEST(test_game_over_top) {
  GameInfo_t game;
  InitGame(&game);

  // Заполняем верхний ряд
  for (int x = 0; x < FIELD_WIDTH; x++) game.field[0][x] = 101;

  int running = 1;
  int over = CheckGameOver(&game, &running);

  ck_assert_int_eq(over, 1);
  ck_assert_int_eq(running, 0);

  FreeGame(&game);
}
END_TEST

START_TEST(test_score) {
  GameInfo_t game;
  game.field = CreateZeroField(FIELD_WIDTH, FIELD_HEIGHT);
  game.next = CreateZeroField(4, 4);
  game.score = 0;
  game.level = 1;
  game.pause = 0;

  AddScore(&game, 1);
  ck_assert_int_eq(game.score, 100);

  AddScore(&game, 2);
  ck_assert_int_eq(game.score, 100 + 300);

  AddScore(&game, 3);
  ck_assert_int_eq(game.score, 400 + 700);

  AddScore(&game, 4);
  ck_assert_int_eq(game.score, 1100 + 1500);

  FreeGame(&game);
}
END_TEST

START_TEST(test_level_10_reached) {
  GameInfo_t game;
  game.field = CreateZeroField(FIELD_WIDTH, FIELD_HEIGHT);
  game.next = CreateZeroField(4, 4);
  game.score = 6300;  // выше 10 уровня
  game.level = 1;
  game.pause = 0;

  // AddScore вызовет проверку уровня
  AddScore(&game, 4);

  ck_assert_int_eq(game.level, 10);

  FreeGame(&game);
}
END_TEST

START_TEST(test_move_down) {
  GameInfo_t game;
  game.field = CreateZeroField(FIELD_WIDTH, FIELD_HEIGHT);
  game.next = CreateZeroField(4, 4);
  game.score = 0;
  game.level = 1;
  game.pause = 0;

  // Рисуем I-фигуру горизонтально вручную
  game.field[0][3] = 1;
  game.field[0][4] = 1;
  game.field[0][5] = 1;
  game.field[0][6] = 1;

  // Вызываем RealUpdateCurrentState несколько раз
  // чтобы сработал счётчик кадров
  for (int i = 0; i < 35; i++) {  // больше чем frame_delays[1] = 30
    RealUpdateCurrentState(&game);
  }

  // Проверяем, что фигура сместилась вниз
  ck_assert_int_eq(game.field[0][3], 0);  // старая позиция пуста
  ck_assert_int_eq(game.field[1][3], 1);  // новая позиция занята
  ck_assert_int_eq(game.field[1][4], 1);
  ck_assert_int_eq(game.field[1][5], 1);
  ck_assert_int_eq(game.field[1][6], 1);

  FreeGame(&game);
}
END_TEST

Suite* tetris_suite(void) {
  Suite* s = suite_create("Tetris");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_init);
  tcase_add_test(tc, test_left);
  tcase_add_test(tc, test_right);
  tcase_add_test(tc, test_rotate);
  tcase_add_test(tc, test_drop);
  tcase_add_test(tc, test_pause);
  tcase_add_test(tc, test_game_over_top);
  tcase_add_test(tc, test_score);
  tcase_add_test(tc, test_move_down);
  tcase_add_test(tc, test_level_10_reached);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  int failed = 0;
  Suite* s = tetris_suite();
  SRunner* sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return failed;
}