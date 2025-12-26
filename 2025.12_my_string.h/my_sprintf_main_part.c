#include "my_sprintf_main_part.h"

#include <ctype.h>
#include <limits.h>
#include <stdint.h>  // для uintptr_t - в Хех
#include <stdio.h>   // отладка
#include <string.h>

#include "my_sprintf_float.h"

int my_sprintf(char* str, const char* format, ...) {
  va_list args;
  va_start(args, format);

  char* str_ptr = str;
  int total_written = 0;

  while (*format) {
    if (*format != '%') {
      *str_ptr++ = *format++;
      total_written++;
    } else {
      format++;
      format_set params = {0};
      parse_format(&format, &params, &args, &str_ptr, &total_written);
    }
  }

  *str_ptr = '\0';
  va_end(args);
  return total_written;
}

void parse_format(const char** format, format_set* params, va_list* args,
                  char** str_ptr, int* total_written) {
  params->precision = -1;
  bool expect_precision = false;
  bool continue_parsing = true;
  while (continue_parsing) {
    char c = **format;

    if (expect_precision && isdigit(c)) {
      params->precision = parse_number(format);
      expect_precision = false;
    } else if (c == '.') {
      expect_precision = true;
      params->precision = 0;
      (*format)++;
    } else if (c == '-' || c == '+' || c == ' ' || c == '0' || c == '#') {
      chose_flag(c, params);
      (*format)++;
    } else if (isdigit(c)) {
      params->width = parse_number(format);
    } else if (c == '*') {
      if (expect_precision) {
        take_precision_from_variable(params, args);
      } else {
        take_width_from_variable(params, args);
      }
      (*format)++;
    } else if (c == 'h' || c == 'l' || c == 'L') {
      params->length = c;
      (*format)++;
    } else {
      continue_parsing = false;
    }
  }
  choose_specifier(format, params, args, str_ptr, total_written);
}

void chose_flag(char c, format_set* params) {
  switch (c) {
    case '-':
      params->minus = true;
      break;
    case '+':
      params->plus = true;
      break;
    case ' ':
      params->space = true;
      break;
    case '0':
      params->zero = true;
      break;
    case '#':
      params->hashteg = true;
      break;
  }
}

int parse_number(const char** format) {
  int number = 0;
  while (isdigit(**format)) {
    number = number * 10 + (**format - '0');
    (*format)++;
  }

  return number;
}

void take_width_from_variable(format_set* params, va_list* args) {
  params->width = va_arg(*args, int);
  if (params->width < 0) {
    params->minus = true;
    params->width = -1 * params->width;
  }
}

void take_precision_from_variable(format_set* params, va_list* args) {
  int precision = va_arg(*args, int);
  if (precision < 0) {
    params->precision = -1;
  } else {
    params->precision = precision;
  }
}

void choose_specifier(const char** format, format_set* params, va_list* args,
                      char** str_ptr, int* total_written) {
  switch (**format) {
    case 'c':
      use_specifier_c(str_ptr, total_written, params, args);
      break;
    case 'f':
      use_specifier_f(str_ptr, total_written, params, args);
      break;
    case 's':
      choose_function_for_specifier_s(str_ptr, total_written, params, args);
      break;
    case 'd':
      use_specifier_d(str_ptr, total_written, params, args);
      break;
    case 'u':
      use_specifier_u(str_ptr, total_written, params, args);
      break;
    case '%':
      add_char(str_ptr, total_written, '%');
      break;
    case 'g':
    case 'G':
      use_specifier_g(str_ptr, total_written, params, args, **format == 'G');
      break;
    case 'e':
    case 'E':
      use_specifier_e(str_ptr, total_written, params, args, **format == 'E');
      break;
    case 'x':
      use_specifier_x(str_ptr, total_written, params, args, false);
      break;
    case 'X':
      use_specifier_x(str_ptr, total_written, params, args, true);
      break;
    case 'o':
      use_specifier_o(str_ptr, total_written, params, args);
      break;
    case 'p':
      use_specifier_p(str_ptr, total_written, params, args);
      break;
  }
  (*format)++;
}

int calculate_precision_and_length(const format_set* params, char* num_buffer,
                                   int num, int* base_num_len) {
  if (params->precision == 0 && num == 0) {
    num_buffer[0] = '\0';
  }
  *base_num_len = strlen(num_buffer);

  return (params->precision > (*base_num_len))
             ? (params->precision - *base_num_len)
             : 0;
}

void use_specifier_c(char** str_ptr, int* total_written,
                     const format_set* params, va_list* args) {
  int c = va_arg(*args, int);
  int content_length = 1;
  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  if (!params->minus) {
    add_spaces(str_ptr, total_written, width_spaces);
  }
  add_char(str_ptr, total_written, (char)c);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void use_specifier_d(char** str_ptr, int* total_written, format_set* params,
                     va_list* args) {
  check_precision(params, 1);
  long long num;
  get_variable(params, args, &num);
  bool is_negative = (num < 0);
  bool is_llong_min = (num == LLONG_MIN);
  char num_buffer[20];
  if (is_llong_min) {
    strncpy(num_buffer, "9223372036854775808", 20);
  } else {
    if (is_negative) {
      num = -num;
    }
    char* run_along_line_with_number = num_buffer;
    my_lltoa_base_10(num, &run_along_line_with_number);
    revers_line_from_toa(run_along_line_with_number, num_buffer);
  }
  bool need_plus = params->plus && !is_negative;
  bool need_space = params->space && !is_negative;
  char sign_char =
      is_negative ? '-' : (need_plus ? '+' : (need_space ? ' ' : '\0'));
  int base_num_len;
  int precision_zeros =
      calculate_precision_and_length(params, num_buffer, num, &base_num_len);
  int content_length = (sign_char ? 1 : 0) + precision_zeros + base_num_len;
  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  if (sign_char) {
    add_char(str_ptr, total_written, sign_char);
  }
  add_number_with_padding(str_ptr, total_written, params, num_buffer,
                          precision_zeros, width_spaces);
}

void choose_function_for_specifier_s(char** str_ptr, int* total_written,
                                     const format_set* params, va_list* args) {
  if (params->length == 'l') {
    wchar_t* wide_str = va_arg(*args, wchar_t*);
    use_specifier_wide_s(str_ptr, total_written, params, wide_str);
  } else {
    const char* str = va_arg(*args, char*);
    use_specifier_s(str_ptr, total_written, params, str);
  }
}

void use_specifier_wide_s(char** str_ptr, int* total_written,
                          const format_set* params, wchar_t* wide_str) {
  if (wide_str == NULL) {
    use_specifier_s(str_ptr, total_written, params, NULL);
  } else {
    int wide_len = 0;
    while (wide_str[wide_len] != 0) {
      wide_len++;
    }
    if (params->precision != -1 && params->precision < wide_len) {
      wide_len = params->precision;
    }
    char ascii_buffer[256] = {0};
    for (int i = 0; i < wide_len && i < 255; i++) {
      if (wide_str[i] <= 127) {
        ascii_buffer[i] = (char)wide_str[i];
      } else {
        ascii_buffer[i] = '?';
      }
    }
    ascii_buffer[wide_len] = '\0';
    use_specifier_s(str_ptr, total_written, params, ascii_buffer);
  }
}

void use_specifier_s(char** str_ptr, int* total_written,
                     const format_set* params, const char* str) {
  bool is_null = false;
  if (str == NULL) {
    str = "(null)";
    is_null = true;
  }
  int str_len = strlen(str);
  int output_len = str_len;
  if (params->precision != -1) {
    if (is_null && params->precision < 6) {
      output_len = 0;
    } else if (params->precision < str_len) {
      output_len = params->precision;
    }
  }
  int width_spaces =
      (params->width > output_len) ? (params->width - output_len) : 0;

  if (!params->minus) {
    add_spaces(str_ptr, total_written, width_spaces);
  }
  for (int i = 0; i < output_len; i++) {
    add_char(str_ptr, total_written, str[i]);
  }
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void use_specifier_u(char** str_ptr, int* total_written, format_set* params,
                     va_list* args) {
  check_precision(params, 1);
  unsigned long long num;
  get_unsigned_variable(params, args, &num);

  char num_buffer[21];
  char* run_along_line_with_number = num_buffer;
  my_ulltoa(num, &run_along_line_with_number, 10);
  revers_line_from_toa(run_along_line_with_number, num_buffer);
  int base_num_len;
  int precision_zeros =
      calculate_precision_and_length(params, num_buffer, num, &base_num_len);
  int content_length = precision_zeros + base_num_len;
  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  add_number_with_padding(str_ptr, total_written, params, num_buffer,
                          precision_zeros, width_spaces);
}

void add_number_with_padding(char** str_ptr, int* total_written,
                             const format_set* params, const char* num_buffer,
                             int precision_zeros, int width_spaces) {
  add_left_spaces(params, str_ptr, total_written, width_spaces);
  add_zeros(str_ptr, total_written, precision_zeros);
  add_number(str_ptr, total_written, num_buffer);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void add_left_spaces(const format_set* params, char** str_ptr,
                     int* total_written, int width_spaces) {
  if (!params->minus) {
    if (params->zero) {
      add_zeros(str_ptr, total_written, width_spaces);
    } else {
      add_spaces(str_ptr, total_written, width_spaces);
    }
  }
}

void get_variable(const format_set* params, va_list* args, long long* num) {
  switch (params->length) {
    case 'h':
      *num = (short)va_arg(*args, int);
      break;
    case 'l':
      *num = va_arg(*args, long);
      break;
    default:
      *num = va_arg(*args, int);
      break;
  }
}

void get_unsigned_variable(const format_set* params, va_list* args,
                           unsigned long long* num) {
  switch (params->length) {
    case 'h':
      *num = (unsigned short)va_arg(*args, int);
      break;
    case 'l':
      *num = va_arg(*args, unsigned long);
      break;
    default:
      *num = va_arg(*args, unsigned int);
      break;
  }
}

void my_ulltoa(unsigned long long value, char** run_along_line_with_number,
               int base) {
  do {
    *(*run_along_line_with_number)++ = '0' + (value % base);
    value /= base;
  } while (value > 0);
}

void use_specifier_p(char** str_ptr, int* total_written,
                     const format_set* params, va_list* args) {
  void* ptr = va_arg(*args, void*);
  char ptr_buffer[19] = {0};
  bool need_0x_prefix = true;

  if (ptr == NULL) {
    strncpy(ptr_buffer, NULL_PTR_STRING, sizeof(ptr_buffer) - 1);
    need_0x_prefix = false;
  } else {
    char* run_along_line_with_number = ptr_buffer;
    uintptr_t ptr_value = (uintptr_t)ptr;
    my_ulltoa_with_two_registers(ptr_value, &run_along_line_with_number, false,
                                 "0123456789abcdef", NULL);
    revers_line_from_toa(run_along_line_with_number, ptr_buffer);
  }
  int content_length = strlen(ptr_buffer);
  if (need_0x_prefix) {
    content_length += 2;
  }

  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;

  if (!params->minus) {
    add_spaces(str_ptr, total_written, width_spaces);
  }
  if (need_0x_prefix) {
    add_char(str_ptr, total_written, '0');
    add_char(str_ptr, total_written, 'x');
  }
  add_number(str_ptr, total_written, ptr_buffer);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void my_ulltoa_with_two_registers(unsigned long long value,
                                  char** run_along_line_with_number,
                                  bool uppercase, const char* digits_lower,
                                  const char* digits_upper) {
  const char* digits = uppercase ? digits_upper : digits_lower;

  do {
    *(*run_along_line_with_number)++ = digits[value % 16];
    value /= 16;
  } while (value > 0);
}

void use_specifier_o(char** str_ptr, int* total_written, format_set* params,
                     va_list* args) {
  check_precision(params, 1);
  unsigned long long num;
  get_unsigned_variable(params, args, &num);
  char num_buffer[25];
  char* run_along_line_with_number = num_buffer;
  my_ulltoa(num, &run_along_line_with_number, 8);
  revers_line_from_toa(run_along_line_with_number, num_buffer);
  bool add_octal_prefix = params->hashteg && num != 0;
  int base_num_len;
  int precision_zeros =
      calculate_precision_and_length(params, num_buffer, num, &base_num_len);
  int content_length =
      precision_zeros + base_num_len + (add_octal_prefix ? 1 : 0);
  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  add_left_spaces(params, str_ptr, total_written, width_spaces);
  if (add_octal_prefix) {
    add_char(str_ptr, total_written, '0');
  }
  add_zeros(str_ptr, total_written, precision_zeros);
  add_number(str_ptr, total_written, num_buffer);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void use_specifier_x(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase) {
  check_precision(params, 1);
  unsigned long long num;
  get_unsigned_variable(params, args, &num);
  char num_buffer[25];
  char* run_along_line_with_number = num_buffer;
  const char* digits_lower = "0123456789abcdef";
  const char* digits_upper = "0123456789ABCDEF";
  my_ulltoa_with_two_registers(num, &run_along_line_with_number, uppercase,
                               digits_lower, digits_upper);
  revers_line_from_toa(run_along_line_with_number, num_buffer);
  bool add_hex_prefix = params->hashteg && num != 0;
  int base_num_len;
  int precision_zeros =
      calculate_precision_and_length(params, num_buffer, num, &base_num_len);
  int content_length =
      precision_zeros + base_num_len + (add_hex_prefix ? 2 : 0);
  int width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  add_left_spaces(params, str_ptr, total_written, width_spaces);
  if (add_hex_prefix) {
    add_char(str_ptr, total_written, '0');
    add_char(str_ptr, total_written, uppercase ? 'X' : 'x');
  }
  add_zeros(str_ptr, total_written, precision_zeros);
  add_number(str_ptr, total_written, num_buffer);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}