#include "my_sprintf_common.h"

void check_precision(format_set* params, int default_precision) {
  if (params->precision == -1) {
    params->precision = default_precision;
  }
}

void add_char(char** str_ptr, int* total_written, char c) {
  *(*str_ptr)++ = c;
  (*total_written)++;
}

void add_spaces(char** str_ptr, int* total_written, int count) {
  for (int i = 0; i < count; i++) {
    add_char(str_ptr, total_written, ' ');
  }
}

void add_zeros(char** str_ptr, int* total_written, int count) {
  for (int i = 0; i < count; i++) {
    add_char(str_ptr, total_written, '0');
  }
}

void add_number(char** str_ptr, int* total_written, const char* buffer) {
  while (*buffer) {
    add_char(str_ptr, total_written, *buffer++);
  }
}

void add_right_spaces(const format_set* params, char** str_ptr,
                      int* total_written, int width_spaces) {
  if (params->minus) {
    add_spaces(str_ptr, total_written, width_spaces);
  }
}

void my_lltoa_base_10(long long value, char** run_along_line_with_number) {
  do {
    *(*run_along_line_with_number)++ = '0' + (value % 10);
    value /= 10;
  } while (value > 0);
}

void revers_line_from_toa(char* end, char* start) {
  *end = '\0';

  end--;
  while (start < end) {
    char temp = *start;
    *start = *end;
    *end = temp;
    start++;
    end--;
  }
}