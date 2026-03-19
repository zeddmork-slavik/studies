#include "utils_internal.h"

static void calculate_digits(int* has_digits, int* digits, uint32_t temp[3]) {
  *has_digits = 0;
  uint64_t remainder = divide_96_by_10_with_remainder(temp);
  for (int i = 0; i < 3 && !*has_digits; i++) {
    if (temp[i] != 0) {
      *has_digits = 1;
    }
  }
  if (*has_digits == 1 || remainder != 0) {
    (*digits)++;
  }
}

static int calculate_digits_to_drop_in_96bit(const uint32_t value[3]) {
  uint32_t temp[3];
  for (int i = 0; i < 3; i++) {
    temp[i] = value[i];
  }

  int digits = 0;
  int is_zero = 1;

  for (int i = 0; i < 3; i++) {
    if (temp[i] != 0) {
      is_zero = 0;
    }
  }

  if (!is_zero) {
    int has_digits = 1;
    while (has_digits) {
      calculate_digits(&has_digits, &digits, temp);
    }
  }
  return (digits > 7) ? (digits - 7) : 0;
}

void round_decimal_to_7_digits(decimal* d) {
  uint32_t mantissa[3] = {d->bits[0], d->bits[1], d->bits[2]};

  int drop_digits = calculate_digits_to_drop_in_96bit(mantissa);

  if (drop_digits) {
    decimal temp = *d;
    decimal quotient = {0};
    int has_more_digits = 0;
    int remainder = 0;

    for (int i = 0; i < drop_digits - 1; i++) {
      if (remainder != 0) {
        has_more_digits = 1;
      }
      divide_by_10_with_remainder(temp, &quotient, &remainder);
      temp = quotient;
    }
    divide_by_10_with_remainder(temp, &quotient, &remainder);
    round_bankers(&quotient, remainder, has_more_digits);

    for (int i = 0; i < drop_digits; i++) {
      multiply_by_10(&quotient);
    }
    *d = quotient;
  }
}

void multiply_320_by_5(uint32_t value[10]) {
  uint64_t carry = 0;
  for (int i = 0; i < 10; i++) {
    uint64_t product = (uint64_t)value[i] * 5 + carry;
    value[i] = (uint32_t)(product & 0xFFFFFFFF);
    carry = product >> 32;
  }
}

uint32_t divide_320_by_10(uint32_t value[10]) {
  uint64_t remainder = 0;
  for (int i = 9; i >= 0; i--) {
    uint64_t current = ((uint64_t)remainder << 32) | value[i];
    value[i] = (uint32_t)(current / 10);
    remainder = current % 10;
  }

  return (uint32_t)remainder;
}
