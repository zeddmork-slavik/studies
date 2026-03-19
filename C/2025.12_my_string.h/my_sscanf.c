#include "my_sscanf.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int my_sscanf(const char* str, const char* format, ...) {
  va_list arg;
  va_start(arg, format);

  int count_variables = 0;
  const char* str_ptr = str;
  bool should_continue = true;
  bool input_failure = false;

  if (str == NULL || *str == '\0') {
    va_end(arg);
    should_continue = false;
    input_failure = true;
  }

  while (*format && should_continue) {
    if (isspace(*format)) {
      skip_whitespace_characters(&format, NULL);
      skip_whitespace_characters(&str_ptr, &input_failure);
    } else if (*format == '%') {
      format++;

      if (*format == '%') {
        skip_whitespace_characters(&str_ptr, &input_failure);
        should_continue = compare_literal(&format, &str_ptr);
      } else {
        should_continue = conversion_specification(
            &format, &str_ptr, str, &arg, &count_variables, &input_failure);
      }
    } else {
      should_continue = compare_literal(&format, &str_ptr);
    }
    if (should_continue && *str_ptr == '\0' && *format != '\0' &&
        !(*format == '%' && *(format + 1) == 'n')) {
      input_failure = true;
      should_continue = false;
    }
  }

  va_end(arg);
  return count_variables ? count_variables
                         : (input_failure ? EOF : count_variables);
}

void skip_whitespace_characters(const char** str, bool* input_failure) {
  while (isspace(**str)) {
    (*str)++;
  }

  if (**str == '\0' && input_failure != NULL) {
    *input_failure = true;
  }
}

bool compare_literal(const char** format, const char** str) {
  bool success = (**format == **str);
  *format += success;
  *str += success;
  return success;
}

bool conversion_specification(const char** format, const char** str,
                              const char* original_str, va_list* arg,
                              int* count, bool* input_failure) {
  bool read_success = false;
  bool assign_success = false;

  bool skip = false;

  if (**format == '*') {
    skip = true;
    (*format)++;
  }

  int width = 0;
  while (isdigit(**format)) {
    width = width * 10 + (**format - '0');
    (*format)++;
  }

  char length_modifier = 0;
  if (**format == 'h' || **format == 'l' || **format == 'L') {
    length_modifier = **format;
    (*format)++;
  }
  s_choose_specifier(format, &read_success, str, arg, skip, width,
                     &assign_success, original_str, length_modifier,
                     input_failure);

  if (assign_success) {
    (*count)++;
  }
  if (read_success && skip && **str == '\0' && **format != '\0' &&
      !(**format == '%' && *(*format + 1) == 'n')) {
    *input_failure = true;
  }
  return read_success;
}

void s_choose_specifier(const char** format, bool* read_success,
                        const char** str, va_list* arg, bool skip, int width,
                        bool* assign_success, const char* original_str,
                        char length_modifier, bool* input_failure) {
  switch (**format) {
    case 'c':
      (*format)++;
      *read_success = s_use_specifier_c(str, arg, skip, width, assign_success,
                                        length_modifier);
      break;
    case 'd':
      (*format)++;
      *read_success = s_use_specifier_d(str, arg, skip, width, assign_success,
                                        length_modifier, input_failure);
      break;
    case 'i':
      (*format)++;
      *read_success = use_specifier_i(str, arg, skip, width, assign_success,
                                      length_modifier, input_failure);
      break;
    case 'e':
    case 'E':
    case 'f':
    case 'g':
    case 'G':
      (*format)++;
      *read_success = use_specifier_float(str, arg, skip, width, assign_success,
                                          length_modifier, input_failure);
      break;
    case 'o':
      (*format)++;
      *read_success = s_use_specifier_o(str, arg, skip, width, assign_success,
                                        length_modifier, input_failure);
      break;
    case 's':
      (*format)++;
      *read_success = s_use_specifier_s(str, arg, skip, width, assign_success,
                                        length_modifier, input_failure);
      break;
    default:
      switch_second_part(format, read_success, str, arg, skip, width,
                         assign_success, original_str, length_modifier,
                         input_failure);
      break;
  }
}

void switch_second_part(const char** format, bool* read_success,
                        const char** str, va_list* arg, bool skip, int width,
                        bool* assign_success, const char* original_str,
                        char length_modifier, bool* input_failure) {
  switch (**format) {
    case 'u':
      (*format)++;
      *read_success = s_use_specifier_u(str, arg, skip, width, assign_success,
                                        length_modifier, input_failure);
      break;
    case 'x':
    case 'X':
      (*format)++;
      *read_success = s_use_specifier_x(str, arg, skip, width, assign_success,
                                        length_modifier, input_failure);
      break;
    case 'p':
      (*format)++;
      *read_success = s_use_specifier_p(str, arg, skip, width, assign_success,
                                        input_failure);
      break;
    case 'n':
      (*format)++;
      *read_success = true;
      use_specifier_n(str, arg, original_str);
      break;
  }
}

void convert_signed_integer_result(va_list* arg, bool skip,
                                   bool* assign_success, char length_modifier,
                                   long value) {
  if (!skip) {
    switch (length_modifier) {
      case 'h': {
        short* target = va_arg(*arg, short*);
        *target = (short)value;
        break;
      }
      case 'l': {
        long* target = va_arg(*arg, long*);
        *target = value;
        break;
      }
      default: {
        int* target = va_arg(*arg, int*);
        *target = (int)value;
        break;
      }
    }
    *assign_success = true;
  }
}

void convert_unsigned_integer_result(va_list* arg, bool skip,
                                     bool* assign_success, char length_modifier,
                                     unsigned long value) {
  if (!skip) {
    switch (length_modifier) {
      case 'h': {
        unsigned short* target = va_arg(*arg, unsigned short*);
        *target = (unsigned short)value;
        break;
      }
      case 'l': {
        unsigned long* target = va_arg(*arg, unsigned long*);
        *target = value;
        break;
      }
      default: {
        unsigned int* target = va_arg(*arg, unsigned int*);
        *target = (unsigned int)value;
        break;
      }
    }
    *assign_success = true;
  }
}

void check_width(int* width, int default_width) {
  if (*width == 0) {
    *width = default_width;
  } else {
    (*width)++;
  }
}

void prepare_number_buffer(const char** str, int width, char* temp_buffer) {
  const char* current = *str;
  strncpy(temp_buffer, current, width - 1);
  temp_buffer[width] = '\0';
}

int parse_signed_number(const char* temp_buffer, int base, long* value) {
  char* endptr;
  *value = strtol(temp_buffer, &endptr, base);
  return endptr - temp_buffer;
}

int parse_unsigned_number(const char* temp_buffer, int base,
                          unsigned long* value) {
  char* endptr;
  *value = strtoul(temp_buffer, &endptr, base);
  return endptr - temp_buffer;
}

void finalize_signed_conversion(const char** str, va_list* arg, bool skip,
                                bool* assign_success, char length_modifier,
                                int chars_used, long value, bool* success) {
  if (chars_used > 0) {
    *str += chars_used;
    convert_signed_integer_result(arg, skip, assign_success, length_modifier,
                                  value);
    *success = true;
  }
}

void finalize_unsigned_conversion(const char** str, va_list* arg, bool skip,
                                  bool* assign_success, char length_modifier,
                                  int chars_used, unsigned long value,
                                  bool* success) {
  if (chars_used > 0) {
    *str += chars_used;
    convert_unsigned_integer_result(arg, skip, assign_success, length_modifier,
                                    value);
    *success = true;
  }
}

bool s_use_specifier_c(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier) {
  bool success = false;
  if (width == 0) {
    width = 1;
  }
  if (**str != '\0') {
    success = true;
    if (!skip) {
      copy_chars_to_target(str, arg, width, assign_success, length_modifier);
    }
    *str += width;
  }

  return success;
}

void copy_chars_to_target(const char** str, va_list* arg, int width,
                          bool* assign_success, char length_modifier) {
  if (length_modifier == 'l') {
    wchar_t* target = va_arg(*arg, wchar_t*);

    for (int i = 0; i < width && (*str)[i] != '\0'; i++) {
      target[i] = (wchar_t)(*str)[i];
    }
  } else {
    char* target = va_arg(*arg, char*);
    for (int i = 0; i < width && (*str)[i] != '\0'; i++) {
      target[i] = (*str)[i];
    }
  }
  *assign_success = true;
}

bool s_use_specifier_d(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure) {
  bool read_success = false;
  check_width(&width, 21);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  memset(temp_buffer, 0, width);
  prepare_number_buffer(str, width, temp_buffer);
  long value = 0;
  int chars_used = parse_signed_number(temp_buffer, 10, &value);
  finalize_signed_conversion(str, arg, skip, assign_success, length_modifier,
                             chars_used, value, &read_success);
  return read_success;
}

bool use_specifier_i(const char** str, va_list* arg, bool skip, int width,
                     bool* assign_success, char length_modifier,
                     bool* input_failure) {
  bool success = false;
  check_width(&width, 23);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);
  long value = 0;
  int chars_used = parse_signed_number(temp_buffer, 0, &value);
  finalize_signed_conversion(str, arg, skip, assign_success, length_modifier,
                             chars_used, value, &success);
  return success;
}

bool use_specifier_float(const char** str, va_list* arg, bool skip, int width,
                         bool* assign_success, char length_modifier,
                         bool* input_failure) {
  bool success = false;
  check_width(&width, 51);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);
  char* endptr;
  long double value;
  if (length_modifier == 'L') {
    value = strtold(temp_buffer, &endptr);
  } else {
    value = strtod(temp_buffer, &endptr);
  }
  int chars_used = endptr - temp_buffer;
  if (chars_used > 0) {
    *str += chars_used;
    convert_float_result(arg, skip, assign_success, length_modifier, value);
    success = true;
  }
  return success;
}

void convert_float_result(va_list* arg, bool skip, bool* assign_success,
                          char length_modifier, long double value) {
  if (!skip) {
    switch (length_modifier) {
      case 'L': {
        long double* target = va_arg(*arg, long double*);
        *target = value;
        break;
      }
      case 'l': {
        double* target = va_arg(*arg, double*);
        *target = (double)value;
        break;
      }
      default: {
        float* target = va_arg(*arg, float*);
        *target = (float)value;
        break;
      }
    }
    *assign_success = true;
  }
}

bool s_use_specifier_o(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure) {
  bool success = false;
  check_width(&width, 23);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);
  unsigned long value;
  int chars_used = parse_unsigned_number(temp_buffer, 8, &value);
  finalize_unsigned_conversion(str, arg, skip, assign_success, length_modifier,
                               chars_used, value, &success);
  return success;
}

bool s_use_specifier_s(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure) {
  bool success = false;

  skip_whitespace_characters(str, input_failure);

  if (**str == '\0') {
    success = false;
  } else {
    int max_chars = (width > 0) ? width : 1000;

    if (length_modifier == 'l') {
      wchar_t* target = skip ? NULL : va_arg(*arg, wchar_t*);
      success = read_wide_line(str, max_chars, target, skip, assign_success);
    } else {
      char* target = skip ? NULL : va_arg(*arg, char*);
      success = read_line(str, max_chars, target, skip, assign_success);
    }
  }

  return success;
}

bool read_line(const char** str, int max_chars, char* target, bool skip,
               bool* assign_success) {
  int chars_read = 0;

  while (chars_read < max_chars && **str != '\0' && !isspace(**str)) {
    if (!skip) {
      target[chars_read] = **str;
    }
    (*str)++;
    chars_read++;
  }

  if (chars_read > 0 && !skip) {
    target[chars_read] = '\0';
    *assign_success = true;
  }

  return (chars_read > 0);
}

bool read_wide_line(const char** str, int max_chars, wchar_t* target, bool skip,
                    bool* assign_success) {
  int chars_read = 0;

  while (chars_read < max_chars && **str != '\0' && !isspace(**str)) {
    if (!skip) {
      target[chars_read] = (wchar_t)(**str);
    }
    (*str)++;
    chars_read++;
  }

  if (chars_read > 0 && !skip) {
    target[chars_read] = L'\0';
    *assign_success = true;
  }

  return (chars_read > 0);
}

bool s_use_specifier_u(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure) {
  bool success = false;
  check_width(&width, 21);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);
  unsigned long value;
  int chars_used = parse_unsigned_number(temp_buffer, 10, &value);
  finalize_unsigned_conversion(str, arg, skip, assign_success, length_modifier,
                               chars_used, value, &success);
  return success;
}

bool s_use_specifier_x(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure) {
  bool success = false;
  check_width(&width, 21);
  skip_whitespace_characters(str, input_failure);
  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);
  unsigned long value;
  int chars_used = parse_unsigned_number(temp_buffer, 16, &value);
  finalize_unsigned_conversion(str, arg, skip, assign_success, length_modifier,
                               chars_used, value, &success);
  return success;
}

bool s_use_specifier_p(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, bool* input_failure) {
  bool success = false;
  check_width(&width, 19);

  skip_whitespace_characters(str, input_failure);

  char temp_buffer[width];
  prepare_number_buffer(str, width, temp_buffer);

  char* parse_start = temp_buffer;
  if (parse_start[0] == '0' &&
      (parse_start[1] == 'x' || parse_start[1] == 'X')) {
    parse_start += 2;
  }

  unsigned long value;
  int chars_used = parse_unsigned_number(parse_start, 16, &value);

  if (chars_used > 0) {
    success = true;
    int prefix_length = parse_start - temp_buffer;
    int total_chars_used = prefix_length + chars_used;
    *str += total_chars_used;
    if (!skip) {
      void** target = va_arg(*arg, void**);
      *target = (void*)value;
      *assign_success = true;
    }
  }

  return success;
}

void use_specifier_n(const char** str, va_list* arg, const char* original_str) {
  int* target = va_arg(*arg, int*);
  *target = *str - original_str;
}