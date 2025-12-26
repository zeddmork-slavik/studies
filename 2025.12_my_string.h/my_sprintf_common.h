#ifndef MY_SSPRINTF_COMMON_H
#define MY_SSPRINTF_COMMON_H

#include <stdarg.h>
#include <stdbool.h>

typedef struct {
  bool minus;
  bool plus;
  bool space;
  bool zero;
  bool hashteg;
  int width;
  int precision;
  char length;
} format_set;

void check_precision(format_set *params, int default_precision);
void add_char(char **str_ptr, int *total_written, char c);
void add_spaces(char **str_ptr, int *total_written, int count);
void add_zeros(char **str_ptr, int *total_written, int count);
void add_number(char **str_ptr, int *total_written, const char *buffer);
void add_right_spaces(const format_set *params, char **str_ptr,
                      int *total_written, int width_spaces);
void my_lltoa_base_10(long long value, char **run_along_line_with_number);
void revers_line_from_toa(char *end, char *start);

#endif