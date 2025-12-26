#ifndef MY_SSCANF_H
#define MY_SSCANF_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

int my_sscanf(const char* str, const char* format, ...);
void skip_whitespace_characters(const char** str, bool* input_failure);
bool compare_literal(const char** format, const char** str);
bool conversion_specification(const char** format, const char** str,
                              const char* original_str, va_list* arg,
                              int* count, bool* input_failure);
void s_choose_specifier(const char** format, bool* read_success,
                        const char** str, va_list* arg, bool skip, int width,
                        bool* assign_success, const char* original_str,
                        char length_modifier, bool* input_failure);
void switch_second_part(const char** format, bool* read_success,
                        const char** str, va_list* arg, bool skip, int width,
                        bool* assign_success, const char* original_str,
                        char length_modifier, bool* input_failure);
void convert_signed_integer_result(va_list* arg, bool skip,
                                   bool* assign_success, char length_modifier,
                                   long value);
void convert_unsigned_integer_result(va_list* arg, bool skip,
                                     bool* assign_success, char length_modifier,
                                     unsigned long value);
void check_width(int* width, int default_width);
void prepare_number_buffer(const char** str, int width, char* temp_buffer);
int parse_signed_number(const char* temp_buffer, int base, long* value);
int parse_unsigned_number(const char* temp_buffer, int base,
                          unsigned long* value);
void finalize_signed_conversion(const char** str, va_list* arg, bool skip,
                                bool* assign_success, char length_modifier,
                                int chars_used, long value, bool* success);
void finalize_unsigned_conversion(const char** str, va_list* arg, bool skip,
                                  bool* assign_success, char length_modifier,
                                  int chars_used, unsigned long value,
                                  bool* success);
bool s_use_specifier_c(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier);
void copy_chars_to_target(const char** str, va_list* arg, int width,
                          bool* assign_success, char length_modifier);
bool s_use_specifier_d(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure);
bool use_specifier_i(const char** str, va_list* arg, bool skip, int width,
                     bool* assign_success, char length_modifier,
                     bool* input_failure);
bool use_specifier_float(const char** str, va_list* arg, bool skip, int width,
                         bool* assign_success, char length_modifier,
                         bool* input_failure);
void convert_float_result(va_list* arg, bool skip, bool* assign_success,
                          char length_modifier, long double value);
bool s_use_specifier_o(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure);
bool s_use_specifier_s(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure);
bool read_line(const char** str, int max_chars, char* target, bool skip,
               bool* assign_success);
bool read_wide_line(const char** str, int max_chars, wchar_t* target, bool skip,
                    bool* assign_success);
bool s_use_specifier_u(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure);
bool s_use_specifier_x(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, char length_modifier,
                       bool* input_failure);
bool s_use_specifier_p(const char** str, va_list* arg, bool skip, int width,
                       bool* assign_success, bool* input_failure);
void use_specifier_n(const char** str, va_list* arg, const char* original_str);
char* find_token_end(char* str, const char* delim);
char* skip_delimiters(char* str, const char* delim);

#endif