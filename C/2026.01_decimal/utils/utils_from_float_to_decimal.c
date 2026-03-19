#include <math.h>

#include "utils_internal.h"

void shift_left_96(const uint32_t value[3], int shift, uint32_t result[3]) {
  int word_shift = shift / 32;
  int bit_shift = shift % 32;

  if (word_shift == 0) {
    result[0] = value[0];
    result[1] = value[1];
    result[2] = value[2];
  } else if (word_shift == 1) {
    result[1] = value[0];
    result[2] = value[1];
  } else if (word_shift == 2) {
    result[2] = value[0];
  }

  if (bit_shift > 0) {
    uint64_t carry = 0;
    uint64_t temp;

    temp = ((uint64_t)result[0] << bit_shift);
    result[0] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;

    temp = ((uint64_t)result[1] << bit_shift) | carry;
    result[1] = (uint32_t)(temp & 0xFFFFFFFF);
    carry = temp >> 32;

    temp = ((uint64_t)result[2] << bit_shift) | carry;
    result[2] = (uint32_t)(temp & 0xFFFFFFFF);
  }
}

void float_to_decimal_checks(float src, decimal* dst, int* for_return,
                             int* should_continue) {
  if (dst == NULL || isinf(src) || isnan(src)) {
    *for_return = 1;
    *should_continue = 0;
  } else {
    float abs_src = fabsf(src);

    dst->bits[0] = 0;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    dst->bits[3] = 0;

    if (src == 0.0f) {
      *should_continue = 0;
    }

    else if (abs_src < 1e-28f ||
             (double)abs_src > 79228162514264337593543950335.0) {
      *for_return = 1;
      *should_continue = 0;
    }
  }
}

void handle_fractional_part(uint32_t mant, int real_exp, decimal* result) {
  int pow2_divide = 23 - real_exp;

  uint32_t big_buffer[10] = {0};
  big_buffer[0] = mant;

  for (int i = 0; i < pow2_divide; i++) {
    multiply_320_by_5(big_buffer);
  }

  uint32_t last_remainder = 0;
  int has_more_digits = 0;
  int scale = pow2_divide;

  while (scale > 28) {
    if (last_remainder) {
      has_more_digits = 1;
    }
    last_remainder = divide_320_by_10(big_buffer);
    scale--;
  }

  result->bits[0] = big_buffer[0];
  result->bits[1] = big_buffer[1];
  result->bits[2] = big_buffer[2];
  set_scale(result, scale);

  if (last_remainder) {
    round_bankers(result, last_remainder, has_more_digits);
  }
}