#include "utils/utils_internal.h"

int div(decimal value_1, decimal value_2, decimal* result) {
  int for_return = 0;
  int should_continue = 1;
  check_special_cases(result, value_1, value_2, &for_return, &should_continue);
  if (should_continue) {
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);
    int result_sign = sign1 ^ sign2;
    set_sign(&value_1, 0);
    set_sign(&value_2, 0);
    normalize(&value_1);
    normalize(&value_2);
    int scale1 = get_scale(value_1);
    int scale2 = get_scale(value_2);
    uint32_t mantissa1[3], mantissa2[3];
    get_mantissa_as_array(value_1, mantissa1);
    get_mantissa_as_array(value_2, mantissa2);
    uint32_t quotient_192[6] = {0};
    uint32_t remainder_96[3] = {0};
    uint32_t dividend_192[6] = {0};
    int target_scale = scale1 - scale2;
    multiply_96_to_scale_28(mantissa1, dividend_192, &target_scale);
    div_192_by_96(dividend_192, mantissa2, quotient_192, remainder_96);
    int needed_fractional = 0;
    uint32_t work_mantissa_result[3] = {0};
    for_return = check_too_large_number_and_write_integer_part(
        quotient_192, target_scale, work_mantissa_result, &needed_fractional,
        result_sign);
    if (for_return) {
      should_continue = 0;
    }
    if (should_continue) {
      int scale_diff = target_scale - needed_fractional;
      get_fractional_part_and_rounding(scale_diff, quotient_192, remainder_96,
                                       mantissa2, work_mantissa_result,
                                       needed_fractional, result_sign,
                                       &for_return, should_continue, result);
    }
  }
  return for_return;
}