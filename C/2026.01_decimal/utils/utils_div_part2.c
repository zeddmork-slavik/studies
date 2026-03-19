#include "utils_internal.h"

static int check_all_number_in_first_word(const uint32_t next_quotient_192[6],
                                          int* has_more_digits) {
  int for_return = 1;
  int temp_has_more_digits = 0;
  for (int i = 1; i < 6; i++) {
    if (next_quotient_192[i] != 0) {
      for_return = 0;
      temp_has_more_digits = 1;
    }
  }
  if (for_return && !*has_more_digits) {
    *has_more_digits = temp_has_more_digits;
  }
  return for_return;
}

static void extract_most_significant_digit(const uint32_t next_quotient_192[6],
                                           uint32_t* next_digit,
                                           int* has_more_digits) {
  if (next_quotient_192[0] < 10) {
    *next_digit = next_quotient_192[0];
  } else {
    int temp = next_quotient_192[0];
    while (temp >= 10) {
      temp /= 10;
    }
    *next_digit = temp;
    *has_more_digits = 1;
  }
}

static void use_remainder_96_for_rounding(const uint32_t remainder_96[3],
                                          const uint32_t mantissa2[3],
                                          uint32_t* next_digit,
                                          int* has_more_digits) {
  uint32_t remainder_192[6] = {0};
  int scale_for_remainder = 0;
  multiply_96_to_scale_28(remainder_96, remainder_192, &scale_for_remainder);

  uint32_t next_quotient_192[6] = {0};
  uint32_t next_remainder_96[3] = {0};
  div_192_by_96(remainder_192, mantissa2, next_quotient_192, next_remainder_96);

  if (next_remainder_96[0] != 0 || next_remainder_96[1] != 0 ||
      next_remainder_96[2] != 0) {
    *has_more_digits = 1;
  }
  scale_for_remainder--;
  while (scale_for_remainder) {
    uint32_t last_remainder =
        divide_192_by_10_with_remainder(next_quotient_192);
    scale_for_remainder--;

    if (last_remainder) {
      *has_more_digits = 1;
    }
  }
  int all_number_in_first_word =
      check_all_number_in_first_word(next_quotient_192, has_more_digits);
  if (all_number_in_first_word) {
    extract_most_significant_digit(next_quotient_192, next_digit,
                                   has_more_digits);
  } else {
    while (
        !check_all_number_in_first_word(next_quotient_192, has_more_digits)) {
      uint32_t remainder = divide_192_by_10_with_remainder(next_quotient_192);
      if (remainder != 0) {
        *has_more_digits = 1;
      }
    }
    extract_most_significant_digit(next_quotient_192, next_digit,
                                   has_more_digits);
  }
}

uint32_t divide_96_by_10_with_remainder(uint32_t value[3]) {
  uint32_t remainder = 0;

  for (int i = 2; i >= 0; i--) {
    uint64_t current = ((uint64_t)remainder << 32) | (uint64_t)value[i];
    value[i] = (uint32_t)(current / 10);
    remainder = (uint32_t)(current % 10);
  }
  return remainder;
}

void add_integer_and_fractional_parts(uint32_t result[3], uint32_t addend[3],
                                      uint32_t* next_digit,
                                      int* has_more_digits, int* scale) {
  uint32_t temp_result[3] = {0};
  int overflow = add_96(result, addend, temp_result);
  if (overflow) {
    if (*next_digit) {
      *has_more_digits = 1;
    }
    (*scale)--;
    *next_digit = divide_96_by_10_with_remainder(addend);
    divide_96_by_10_with_remainder(result);
    add_integer_and_fractional_parts(result, addend, next_digit,
                                     has_more_digits, scale);
  } else {
    for (int i = 0; i < 3; i++) {
      result[i] = temp_result[i];
    }
  }
}

void subtract_integer_part(uint32_t quotient_192[6], uint32_t temp_quotient[6],
                           uint32_t work_mantissa_result[3],
                           int* width_fractional_part,
                           int original_target_scale) {
  for (int i = 0; i < *width_fractional_part; i++) {
    multiply_192_by_10(temp_quotient);
  }

  if (compare_192bit_with_max_decimal(temp_quotient)) {
    (*width_fractional_part)--;
    divide_192_by_10_with_remainder(temp_quotient);
  }
  work_mantissa_result[0] = temp_quotient[0];
  work_mantissa_result[1] = temp_quotient[1];
  work_mantissa_result[2] = temp_quotient[2];

  for (int i = *width_fractional_part; i < original_target_scale; i++) {
    multiply_192_by_10(temp_quotient);
  }
  int borrow = 0;
  for (int i = 0; i < 6; i++) {
    uint64_t diff = (uint64_t)quotient_192[i] - temp_quotient[i] - borrow;
    quotient_192[i] = (uint32_t)diff;
    borrow = (diff >> 32) ? 1 : 0;
  }
}

void get_rounding_data(int scale_diff, uint32_t quotient_192[6],
                       uint32_t remainder_96[3], const uint32_t mantissa2[3],
                       uint32_t* next_digit, int* has_more_digits) {
  int next_digit_assigned = 0;

  for (int i = 0; i < scale_diff; i++) {
    uint32_t remainder = divide_192_by_10_with_remainder(quotient_192);
    if (i == scale_diff - 1) {
      *next_digit = remainder;
      next_digit_assigned = 1;
    } else if (remainder != 0) {
      *has_more_digits = 1;
    }
  }
  if (remainder_96[0] != 0 || remainder_96[1] != 0 || remainder_96[2] != 0) {
    if (!next_digit_assigned) {
      use_remainder_96_for_rounding(remainder_96, mantissa2, next_digit,
                                    has_more_digits);
    } else {
      *has_more_digits = 1;
    }
  }
}

void check_special_cases(decimal* result, decimal value_1, decimal value_2,
                         int* for_return, int* should_continue) {
  if (result == NULL) {
    *for_return = 4;
    *should_continue = 0;
  }

  if (is_zero(value_2) && *should_continue) {
    *for_return = 3;
    *should_continue = 0;
  }

  if (is_zero(value_1) && *should_continue) {
    result->bits[0] = 0;
    result->bits[1] = 0;
    result->bits[2] = 0;
    result->bits[3] = value_1.bits[3];
    *should_continue = 0;
  }
}