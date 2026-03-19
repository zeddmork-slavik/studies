#ifndef MY_SSPRINTF_FLOAT_H
#define MY_SSPRINTF_FLOAT_H

#include "my_sprintf_common.h"

#ifdef __linux__
#define NULL_PTR_STRING "(nil)"
#else
#define NULL_PTR_STRING "0x0"
#endif

#define GET_FLOAT_VARIABLE(params, args)                \
  ((params)->length == 'L' ? va_arg(*args, long double) \
                           : (long double)va_arg(*args, double))

void use_specifier_f(char** str_ptr, int* total_written, format_set* params,
                     va_list* args);
void output_formatted_float(char** str_ptr, int* total_written,
                            const format_set* params, const char* num_buffer,
                            char sign_char, int width_spaces,
                            bool use_zero_padding);
void prepare_guidelines_for_float_formatting(const format_set* params,
                                             bool is_negative, bool is_special,
                                             const char* num_buffer,
                                             char* sign_char, int* width_spaces,
                                             bool* use_zero_padding);
void my_ldtoa(long double num, const format_set* params, char* buffer);
void handle_ll_number(long double num, const format_set* params, char* buffer);
void handle_very_large_number(long double num, const format_set* params,
                              char* buffer);
void write_integer_part_to_output(const char* source, char** destination);
void write_fractional_part_to_output(char* buffer, long double fractional,
                                     int precision);
bool check_special_float_cases(long double num, char* buffer);
long double normalize_number(long double num, int* exponent);
void format_scientific_notation(long double mantissa, int exponent,
                                const format_set* params, bool uppercase,
                                char* buffer);
void use_specifier_e(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase);
void use_specifier_g(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase);
void handling_normal_cases_specifier_g(format_set* params, long double* num,
                                       bool* is_negative, bool uppercase,
                                       char* num_buffer);
void remove_trailing_zeros(char* buffer);
void remove_trailing_zeros_scientific(char* buffer, char exp_char);

#endif