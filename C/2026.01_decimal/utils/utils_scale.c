#include "utils_internal.h"

int multiply_by_10(decimal* value) {
  int for_return = 1;

  if (value != NULL) {
    uint32_t const mantissa[3] = {(uint32_t)value->bits[0],
                                  (uint32_t)value->bits[1],
                                  (uint32_t)value->bits[2]};

    uint32_t result[3] = {0, 0, 0};
    uint64_t carry = 0;

    for (int i = 0; i < 3; i++) {
      uint64_t product = (uint64_t)mantissa[i] * 10ULL + carry;
      result[i] = (uint32_t)product;
      carry = product >> 32;
    }

    if (carry == 0) {
      value->bits[0] = (int)result[0];
      value->bits[1] = (int)result[1];
      value->bits[2] = (int)result[2];
      for_return = 0;
    }
  }

  return for_return;
}

int divide_by_10_with_remainder(decimal value, decimal* quotient,
                                int* remainder) {
  int for_return = 0;

  if (quotient != NULL && remainder != NULL) {
    quotient->bits[0] = 0;
    quotient->bits[1] = 0;
    quotient->bits[2] = 0;
    quotient->bits[3] = value.bits[3];
    *remainder = 0;
    for (int i = 2; i >= 0; i--) {
      // Собираем 64-битное число: (remainder умножаем на 2 в 32 - он же из инта
      // постарше) | текущее_слово
      uint64_t current =
          ((uint64_t)(*remainder) << 32) | (uint32_t)value.bits[i];

      quotient->bits[i] = (int)(current / 10);
      *remainder = (int)(current % 10);
    }
  } else {
    for_return = 1;
  }

  return for_return;
}