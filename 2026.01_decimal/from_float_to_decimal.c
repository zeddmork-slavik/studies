#include <string.h>

#include "utils/utils_internal.h"

int from_float_to_decimal(float src, decimal* dst) {
  int for_return = 0;
  int should_continue = 1;
  float_to_decimal_checks(src, dst, &for_return, &should_continue);

  if (should_continue) {
    decimal result = {0};
    uint32_t bits = 0;
    memcpy(&bits, &src, sizeof(uint32_t));
    int sign = (bits >> 31) & 1;
    set_sign(&result, sign);
    int exp = (bits >> 23) & 0xFF;
    uint32_t mant = bits & 0x7FFFFF;

    mant |= 0x800000;

    int real_exp = exp - 127;

    if (real_exp >= 23) {
      int shift = real_exp - 23;

      uint32_t mantissa_array[3] = {mant, 0, 0};
      uint32_t shifted[3] = {0};
      shift_left_96(mantissa_array, shift, shifted);
      from_uint96_to_decimal(shifted, 0, sign, dst);
    } else {
      handle_fractional_part(mant, real_exp, &result);
      copy_decimal(result, dst);
    }
    round_decimal_to_7_digits(dst);
    normalize(dst);
  }
  return for_return;
}