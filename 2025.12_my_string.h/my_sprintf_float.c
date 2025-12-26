#include "my_sprintf_float.h"

#include <limits.h>
#include <math.h>  // для иснан
#include <string.h>

void use_specifier_f(char** str_ptr, int* total_written, format_set* params,
                     va_list* args) {
  check_precision(params, 6);
  long double num = GET_FLOAT_VARIABLE(params, args);
  char num_buffer[100] = {0};
  bool is_negative = false;
  bool is_special = check_special_float_cases(num, num_buffer);

  if (!is_special) {
    if (num < 0) {
      num = -num;
      is_negative = true;
    }
    my_ldtoa(num, params, num_buffer);
  }

  char sign_char;
  int width_spaces;
  bool use_zero_padding;
  prepare_guidelines_for_float_formatting(params, is_negative, is_special,
                                          num_buffer, &sign_char, &width_spaces,
                                          &use_zero_padding);
  output_formatted_float(str_ptr, total_written, params, num_buffer, sign_char,
                         width_spaces, use_zero_padding);
}

void output_formatted_float(char** str_ptr, int* total_written,
                            const format_set* params, const char* num_buffer,
                            char sign_char, int width_spaces,
                            bool use_zero_padding) {
  if (!params->minus) {
    if (use_zero_padding) {
      if (sign_char) {
        add_char(str_ptr, total_written, sign_char);
        sign_char = '\0';
      }
      add_zeros(str_ptr, total_written, width_spaces);
    } else {
      add_spaces(str_ptr, total_written, width_spaces);
    }
  }
  if (sign_char) {
    add_char(str_ptr, total_written, sign_char);
  }
  add_number(str_ptr, total_written, num_buffer);
  add_right_spaces(params, str_ptr, total_written, width_spaces);
}

void prepare_guidelines_for_float_formatting(const format_set* params,
                                             bool is_negative, bool is_special,
                                             const char* num_buffer,
                                             char* sign_char, int* width_spaces,
                                             bool* use_zero_padding) {
  bool need_plus = params->plus && !is_negative;
  bool need_space = params->space && !is_negative && !need_plus;
  *sign_char =
      is_negative ? '-' : (need_plus ? '+' : (need_space ? ' ' : '\0'));

  int content_length = strlen(num_buffer) + (*sign_char ? 1 : 0);
  *width_spaces =
      (params->width > content_length) ? (params->width - content_length) : 0;
  *use_zero_padding = params->zero && !is_special;
}

void my_ldtoa(long double num, const format_set* params, char* buffer) {
  if (num > (long double)LLONG_MAX) {
    handle_very_large_number(num, params, buffer);
  } else {
    handle_ll_number(num, params, buffer);
  }
}

void handle_ll_number(long double num, const format_set* params, char* buffer) {
  long double rounding_additive = 0.5L;
  for (int i = 0; i < params->precision; i++) {
    rounding_additive /= 10.0L;
  }
  num += rounding_additive;
  long long int_part = (long long)num;
  char int_buffer[20] = {0};
  char* run_along_line_with_number = int_buffer;
  if (int_part == 0) {
    *run_along_line_with_number++ = '0';
  } else {
    my_lltoa_base_10(int_part, &run_along_line_with_number);
  }
  revers_line_from_toa(run_along_line_with_number, int_buffer);
  write_integer_part_to_output(int_buffer, &buffer);

  if (params->precision > 0 || (params->hashteg && params->precision == 0)) {
    *buffer++ = '.';
  }
  if (params->precision > 0) {
    long double fractional = num - (long double)int_part;
    write_fractional_part_to_output(buffer, fractional, params->precision);
  }
}

void handle_very_large_number(long double num, const format_set* params,
                              char* buffer) {
  char int_buffer[50] = {0};
  char* run_along_line_with_number = int_buffer;
  long double temp = floorl(num);
  for (int digit_count = 0; temp >= 1.0L && digit_count < 49; digit_count++) {
    long double div_result = temp / 10.0L;
    long long digit =
        (long long)((div_result - floorl(div_result)) * 10.0L + 0.1L);
    *run_along_line_with_number++ = '0' + digit;
    temp = floorl(div_result);
  }
  revers_line_from_toa(run_along_line_with_number, int_buffer);
  write_integer_part_to_output(int_buffer, &buffer);
  if (params->precision > 0 || (params->hashteg && params->precision == 0)) {
    *buffer++ = '.';
  }
  if (params->precision > 0) {
    long double fractional = num - floorl(num);
    write_fractional_part_to_output(buffer, fractional, params->precision);
  }
}

void write_integer_part_to_output(const char* source, char** destination) {
  for (int i = 0; source[i] != '\0'; i++) {
    *(*destination)++ = source[i];
  }
}

void write_fractional_part_to_output(char* buffer, long double fractional,
                                     int precision) {
  for (int i = 0; i < precision; i++) {
    fractional *= 10.0L;
    int digit = (int)fractional;
    *buffer++ = '0' + digit;
    fractional -= (long double)digit;
  }
}

bool check_special_float_cases(long double num, char* buffer) {
  bool is_special = false;
  if (isnan(num)) {
    strncpy(buffer, "nan", 4);
    is_special = true;
  } else if (isinf(num)) {
    if (num < 0) {
      strncpy(buffer, "-inf", 5);
    } else {
      strncpy(buffer, "inf", 4);
    }
    is_special = true;
  }
  return is_special;
}

long double normalize_number(long double num, int* exponent) {
  while (num >= 10.0L) {
    num /= 10.0L;
    (*exponent)++;
  }
  while (num < 1.0L) {
    num *= 10.0L;
    (*exponent)--;
  }
  return num;
}

void format_scientific_notation(long double mantissa, int exponent,
                                const format_set* params, bool uppercase,
                                char* buffer) {
  char mantissa_buffer[50] = {0};
  char exponent_buffer[5] = {0};
  handle_ll_number(mantissa, params, mantissa_buffer);
  char exp_sign = '+';
  if (exponent < 0) {
    exp_sign = '-';
    exponent = -exponent;
  }
  char* run_along_line_with_number = exponent_buffer;
  *run_along_line_with_number++ = exp_sign;
  char* start_exponent_number = run_along_line_with_number;

  if (exponent < 10) {
    *run_along_line_with_number++ = '0';
    *run_along_line_with_number++ = '0' + exponent;
  } else {
    my_lltoa_base_10(exponent, &run_along_line_with_number);
    revers_line_from_toa(run_along_line_with_number, start_exponent_number);
  }
  const char* src = mantissa_buffer;
  while (*src) {
    *buffer++ = *src++;
  }
  *buffer++ = uppercase ? 'E' : 'e';
  src = exponent_buffer;
  while (*src) {
    *buffer++ = *src++;
  }
}

void use_specifier_e(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase) {
  check_precision(params, 6);
  long double num = GET_FLOAT_VARIABLE(params, args);
  char num_buffer[100] = {0};
  bool is_negative = false;
  bool is_special = check_special_float_cases(num, num_buffer);
  if (!is_special) {
    if (signbit(num)) {
      num = -num;
      is_negative = true;
    }
    if (num == 0.0L) {
      format_scientific_notation(0.0L, 0, params, uppercase, num_buffer);
    } else {
      int exponent = 0;
      long double mantissa = normalize_number(num, &exponent);
      format_scientific_notation(mantissa, exponent, params, uppercase,
                                 num_buffer);
    }
  }
  char sign_char;
  int width_spaces;
  bool use_zero_padding;
  prepare_guidelines_for_float_formatting(params, is_negative, is_special,
                                          num_buffer, &sign_char, &width_spaces,
                                          &use_zero_padding);
  output_formatted_float(str_ptr, total_written, params, num_buffer, sign_char,
                         width_spaces, use_zero_padding);
}

void use_specifier_g(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase) {
  check_precision(params, 6);
  long double num = GET_FLOAT_VARIABLE(params, args);
  char num_buffer[100] = {0};
  bool is_negative = false;
  bool is_special = check_special_float_cases(num, num_buffer);
  if (!is_special) {
    handling_normal_cases_specifier_g(params, &num, &is_negative, uppercase,
                                      num_buffer);
  }
  char sign_char;
  int width_spaces;
  bool use_zero_padding;
  prepare_guidelines_for_float_formatting(params, is_negative, is_special,
                                          num_buffer, &sign_char, &width_spaces,
                                          &use_zero_padding);
  output_formatted_float(str_ptr, total_written, params, num_buffer, sign_char,
                         width_spaces, use_zero_padding);
}

void handling_normal_cases_specifier_g(format_set* params, long double* num,
                                       bool* is_negative, bool uppercase,
                                       char* num_buffer) {
  if (*num < 0) {
    *num = -(*num);
    *is_negative = true;
  }
  int exponent = 0;
  long double mantissa = normalize_number(*num, &exponent);
  int precision_choose_style = params->precision;
  if (precision_choose_style == 0) {
    precision_choose_style = 1;
  }
  bool use_f_style = (exponent >= -4 && exponent < precision_choose_style);
  if (use_f_style) {
    params->precision = params->precision - (exponent + 1);
    if (params->precision < 0) params->precision = 0;
    my_ldtoa(*num, params, num_buffer);
    if (!params->hashteg) {
      remove_trailing_zeros(num_buffer);
    }
  } else {
    params->precision = params->precision - 1;
    if (params->precision < 0) {
      params->precision = 0;
    }
    format_scientific_notation(mantissa, exponent, params, uppercase,
                               num_buffer);
    if (!params->hashteg) {
      remove_trailing_zeros_scientific(num_buffer, uppercase ? 'E' : 'e');
    }
  }
}

void remove_trailing_zeros(char* buffer) {
  char* decimal_point = strchr(buffer, '.');
  char* end = buffer + strlen(buffer) - 1;
  if (decimal_point != NULL) {
    while (end > decimal_point && *end == '0') {
      *end = '\0';
      end--;
    }
    if (end == decimal_point) {
      *decimal_point = '\0';
    }
  }
}

void remove_trailing_zeros_scientific(char* buffer, char exp_char) {
  char* exp_pos = strchr(buffer, exp_char);
  char* decimal_point = strchr(buffer, '.');
  char* last_non_zero = exp_pos - 1;
  while (last_non_zero > decimal_point && *last_non_zero == '0') {
    last_non_zero--;
  }
  if (last_non_zero < exp_pos - 1) {
    char* dest = last_non_zero + 1;
    while (*exp_pos != '\0') {
      *dest++ = *exp_pos++;
    }
    *dest = '\0';
  }
  if (last_non_zero == decimal_point) {
    while (decimal_point[1] != '\0') {
      *decimal_point = decimal_point[1];
      decimal_point++;
    }
    *decimal_point = '\0';
  }
}
