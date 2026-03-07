#include "utils_internal.h"

static void count_decimal_digits(uint32_t temp_integer_part[6],
                                 int* integer_digits) {
  int has_digits = 1;
  while (has_digits) {
    has_digits = 0;
    uint64_t remainder = 0;
    int j;

    for (j = 5; j >= 0; j--) {
      uint64_t current =
          ((uint64_t)remainder << 32) | (uint64_t)temp_integer_part[j];
      temp_integer_part[j] = (uint32_t)(current / 10);
      remainder = current % 10;
      if (temp_integer_part[j] != 0) {
        has_digits = 1;
      }
    }
    if (has_digits == 1 || remainder != 0) {
      (*integer_digits)++;
    }
  }
}

static void calculate_width_fractional_part(const uint32_t integer_part[6],
                                            int* width_fractional_part) {
  uint32_t temp_integer_part[6];
  for (int i = 0; i < 6; i++) {
    temp_integer_part[i] = integer_part[i];
  }
  int integer_digits = 0;
  int is_zero = 1;

  for (int i = 0; i < 6; i++) {
    if (temp_integer_part[i] != 0) {
      is_zero = 0;
    }
  }

  if (!is_zero) {
    count_decimal_digits(temp_integer_part, &integer_digits);
  }
  *width_fractional_part = 29 - integer_digits;
  if (*width_fractional_part > 28) {
    *width_fractional_part = 28;
  }
}

int check_too_large_number_and_write_integer_part(
    uint32_t quotient_192[6], int target_scale,
    uint32_t work_mantissa_result[3], int* needed_fractional, int result_sign) {
  uint32_t temp_quotient[6];
  for (int i = 0; i < 6; i++) {
    temp_quotient[i] = quotient_192[i];
  }
  int original_target_scale = target_scale;
  while (target_scale > 0) {
    divide_192_by_10_with_remainder(temp_quotient);
    target_scale--;
  }

  int integer_part_fits = 1;
  for (int i = 3; i < 6 && integer_part_fits; i++) {
    if (temp_quotient[i] != 0) {
      integer_part_fits = 0;
    }
  }
  int for_return = 0;

  if (integer_part_fits) {
    calculate_width_fractional_part(temp_quotient, needed_fractional);
    if (*needed_fractional) {
      subtract_integer_part(quotient_192, temp_quotient, work_mantissa_result,
                            needed_fractional, original_target_scale);
    }
  } else {
    for_return = result_sign ? 2 : 1;
  }
  return for_return;
}

void get_fractional_part_and_rounding(
    const int scale_diff, uint32_t quotient_192[6], uint32_t remainder_96[3],
    const uint32_t mantissa2[3], uint32_t work_mantissa_result[3],
    int needed_fractional, const int result_sign, int* for_return,
    int should_continue, decimal* result) {
  uint32_t next_digit = 0;
  int has_more_digits = 0;

  get_rounding_data(scale_diff, quotient_192, remainder_96, mantissa2,
                    &next_digit, &has_more_digits);
  uint32_t fractional_part_96[3] = {quotient_192[0], quotient_192[1],
                                    quotient_192[2]};
  add_integer_and_fractional_parts(work_mantissa_result, fractional_part_96,
                                   &next_digit, &has_more_digits,
                                   &needed_fractional);
  decimal work_result = {0};
  from_uint96_to_decimal(work_mantissa_result, needed_fractional, result_sign,
                         &work_result);
  int round_result = round_bankers(&work_result, next_digit, has_more_digits);
  if (round_result) {
    *for_return = 1;
    should_continue = 0;
  }
  if (should_continue) {
    if (is_zero(work_result)) {
      set_scale(&work_result, 0);
    }
    normalize(&work_result);
    *result = work_result;
  }
}