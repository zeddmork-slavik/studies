#include "utils_internal.h"

int change_scale_with_rounding(uint32_t big_result[6], int scale, int sign,
                               decimal* result) {
  int for_return = 0;
  int fits_in_96bit = 0;
  int last_remainder = 0;
  int has_more_digits = 0;

  if (!compare_192bit_with_max_decimal(big_result)) {
    fits_in_96bit = 1;
  }

  while (scale > 0 && !fits_in_96bit) {
    uint32_t remainder = divide_192_by_10_with_remainder(big_result);
    scale--;

    if (last_remainder == 0) {
      last_remainder = remainder;
    } else if (remainder != 0) {
      has_more_digits = 1;
    }

    if (!compare_192bit_with_max_decimal(big_result)) {
      fits_in_96bit = 1;
    }
  }
  if (!fits_in_96bit) {
    for_return = sign ? 2 : 1;
  } else {
    uint32_t mantissa[3] = {big_result[0], big_result[1], big_result[2]};

    decimal temp;

    from_uint96_to_decimal(mantissa, scale, sign, &temp);
    if (round_bankers(&temp, last_remainder, has_more_digits)) {
      for_return = sign ? 2 : 1;
    } else {
      normalize(&temp);  // убирает правые нули уменьшая scale
      *result = temp;
    }
  }
  return for_return;
}

static void add_192bit(const uint32_t a[6], const uint32_t b[6],
                       uint32_t result[6]) {
  uint64_t carry = 0;
  for (int i = 0; i < 6; i++) {
    uint64_t sum = (uint64_t)a[i] + (uint64_t)b[i] + carry;
    result[i] = (uint32_t)sum;
    carry = sum >> 32;
  }
}

static void sub_192bit(const uint32_t a[6], const uint32_t b[6],
                       uint32_t result[6]) {
  uint32_t borrow = 0;
  for (int i = 0; i < 6; i++) {
    uint64_t diff = (uint64_t)a[i] - (uint64_t)b[i] - borrow;
    result[i] = (uint32_t)diff;
    borrow = (diff >> 32) ? 1 : 0;
  }
}

static int compare_192bit(const uint32_t a[6], const uint32_t b[6]) {
  int comparison_result = 0;
  for (int i = 5; i >= 0 && comparison_result == 0; i--) {
    if (a[i] > b[i]) {
      comparison_result = 1;
    } else if (a[i] < b[i]) {
      comparison_result = -1;
    }
  }
  return comparison_result;
}

int determine_operation_type(const int sign1, const int sign2) {
  int operation_type = 0;
  if (sign1 == 0 && sign2 == 0) {
    operation_type = 0;
  } else if (sign1 == 1 && sign2 == 1) {
    operation_type = 1;
  } else if (sign1 == 0 && sign2 == 1) {
    operation_type = 2;
  } else if (sign1 == 1 && sign2 == 0) {
    operation_type = 3;
  }
  return operation_type;
}

void increase_scale_to_target(uint32_t big[6], int current_scale,
                              int target_scale) {
  while (current_scale < target_scale) {
    multiply_192_by_10(big);
    current_scale++;
  }
}

void perform_192bit_operation(const uint32_t big1[6], const uint32_t big2[6],
                              int op_type, uint32_t result[6],
                              int* result_sign) {
  switch (op_type) {
    case 0:
      add_192bit(big1, big2, result);
      *result_sign = 0;
      break;

    case 1:
      add_192bit(big1, big2, result);
      *result_sign = 1;
      break;

    case 2: {
      int cmp = compare_192bit(big1, big2);
      if (cmp >= 0) {
        sub_192bit(big1, big2, result);
        *result_sign = 0;
      } else {
        sub_192bit(big2, big1, result);
        *result_sign = 1;
      }
      break;
    }

    case 3: {
      int cmp = compare_192bit(big2, big1);
      if (cmp >= 0) {
        sub_192bit(big2, big1, result);
        *result_sign = 0;
      } else {
        sub_192bit(big1, big2, result);
        *result_sign = 1;
      }
      break;
    }
  }
}