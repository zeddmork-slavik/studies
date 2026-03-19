#ifndef MY_SSPRINTF_MAIN_PART_H
#define MY_SSPRINTF_MAIN_PART_H

#include <stddef.h>  //для wchar_t

#include "my_sprintf_common.h"

int my_sprintf(char* str, const char* format, ...);
void parse_format(const char** format, format_set* params, va_list* args,
                  char** str_ptr, int* total_written);
void chose_flag(char c, format_set* params);
int parse_number(const char** format);
void take_width_from_variable(format_set* params, va_list* args);
void take_precision_from_variable(format_set* params, va_list* args);
void choose_specifier(const char** format, format_set* params, va_list* args,
                      char** str_ptr, int* total_written);
int calculate_precision_and_length(const format_set* params, char* num_buffer,
                                   int num, int* base_num_len);
void use_specifier_c(char** str_ptr, int* total_written,
                     const format_set* params, va_list* args);
void use_specifier_d(char** str_ptr, int* total_written, format_set* params,
                     va_list* args);
void use_specifier_wide_s(char** str_ptr, int* total_written,
                          const format_set* params, wchar_t* wide_str);
void choose_function_for_specifier_s(char** str_ptr, int* total_written,
                                     const format_set* params, va_list* args);
void use_specifier_s(char** str_ptr, int* total_written,
                     const format_set* params, const char* str);
void use_specifier_u(char** str_ptr, int* total_written, format_set* params,
                     va_list* args);
void add_number_with_padding(char** str_ptr, int* total_written,
                             const format_set* params, const char* num_buffer,
                             int precision_zeros, int width_spaces);
void add_left_spaces(const format_set* params, char** str_ptr,
                     int* total_written, int width_spaces);
void get_variable(const format_set* params, va_list* args, long long* num);
void get_unsigned_variable(const format_set* params, va_list* args,
                           unsigned long long* num);
void my_ulltoa(unsigned long long value, char** run_along_line_with_number,
               int base);
void use_specifier_p(char** str_ptr, int* total_written,
                     const format_set* params, va_list* args);
void my_ulltoa_with_two_registers(unsigned long long value,
                                  char** run_along_line_with_number,
                                  bool uppercase, const char* digits_lower,
                                  const char* digits_upper);
void use_specifier_o(char** str_ptr, int* total_written, format_set* params,
                     va_list* args);
void use_specifier_x(char** str_ptr, int* total_written, format_set* params,
                     va_list* args, bool uppercase);

#endif