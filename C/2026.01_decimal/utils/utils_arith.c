#include "utils_internal.h"

int add_96(const uint32_t a[3], const uint32_t b[3], uint32_t result[3]) {
  uint64_t sum = 0;
  uint64_t carry = 0;

  sum = (uint64_t)a[0] + (uint64_t)b[0] + carry;
  result[0] = (uint32_t)sum;
  carry = sum >> 32;

  sum = (uint64_t)a[1] + (uint64_t)b[1] + carry;
  result[1] = (uint32_t)sum;
  carry = sum >> 32;

  sum = (uint64_t)a[2] + (uint64_t)b[2] + carry;
  result[2] = (uint32_t)sum;
  carry = sum >> 32;

  return (carry != 0) ? 1 : 0;
}

static void check_can_subtracted(int* can_subtract, const uint32_t remainder[3],
                                 const uint32_t divisor[3]) {
  if (remainder[2] > divisor[2]) {
    *can_subtract = 1;
  } else if (remainder[2] == divisor[2]) {
    if (remainder[1] > divisor[1]) {
      *can_subtract = 1;
    } else if (remainder[1] == divisor[1]) {
      if (remainder[0] >= divisor[0]) {
        *can_subtract = 1;
      }
    }
  }
}

static void do_subtraction(uint32_t remainder[3], const uint32_t divisor[3],
                           int bit_pos, uint32_t quotient[6]) {
  sub_96(remainder, divisor, remainder);

  int q_bit_pos = bit_pos;
  int q_word_index = q_bit_pos / 32;
  int q_bit_in_word = q_bit_pos % 32;
  SET_BIT(quotient[q_word_index], q_bit_in_word);
}

void div_192_by_96(const uint32_t dividend[6], const uint32_t divisor[3],
                   uint32_t quotient[6], uint32_t remainder[3]) {
  for (int bit_pos = 191; bit_pos >= 0; bit_pos--) {
    uint32_t remainder_97 = 0;
    uint32_t carry0 = GET_BIT(remainder[0], 31);
    uint32_t carry1 = GET_BIT(remainder[1], 31);
    uint32_t carry2 = GET_BIT(remainder[2], 31);
    remainder[0] <<= 1;
    remainder[1] = (remainder[1] << 1) | carry0;
    remainder[2] = (remainder[2] << 1) | carry1;
    remainder_97 = carry2;
    int word_index = bit_pos / 32;
    int bit_in_word = bit_pos % 32;
    uint32_t bit_value = (dividend[word_index] >> bit_in_word) & 1;
    if (bit_value) {
      SET_BIT(remainder[0], 0);
    }
    int can_subtract = 0;
    if (remainder_97) {
      can_subtract = 1;
    } else {
      check_can_subtracted(&can_subtract, remainder, divisor);
    }

    if (can_subtract) {
      do_subtraction(remainder, divisor, bit_pos, quotient);
    }
  }
}

int sub_96(const uint32_t a[3], const uint32_t b[3], uint32_t result[3]) {
  uint64_t borrow = 0;

  for (int i = 0; i < 3; i++) {
    uint64_t temp = (uint64_t)a[i] - (uint64_t)b[i] - borrow;
    result[i] = (uint32_t)temp;
    borrow = (temp > a[i]) ? 1 : 0;
  }

  return 0;
}

uint32_t divide_192_by_10_with_remainder(uint32_t value[6]) {
  uint32_t remainder = 0;

  for (int i = 5; i >= 0; i--) {
    uint64_t current = ((uint64_t)remainder << 32) | (uint64_t)value[i];
    value[i] = (uint32_t)(current / 10);
    remainder = (uint32_t)(current % 10);
  }
  return remainder;
}

void multiply_192_by_10(uint32_t value[6]) {
  uint64_t carry = 0;
  for (int j = 0; j < 6; j++) {
    uint64_t product = (uint64_t)value[j] * 10ULL + carry;
    value[j] = (uint32_t)product;
    carry = product >> 32;
  }
}

void multiply_96_to_scale_28(const uint32_t input[3], uint32_t output[6],
                             int* target_scale) {
  for (int i = 0; i < 6; i++) {
    output[i] = (i < 3) ? input[i] : 0;
  }

  int can_continue = 1;

  while (*target_scale < 28 && can_continue) {
    uint64_t carry = 0;
    uint32_t temp_output[8];

    int j;

    for (j = 0; j < 6; j++) {
      uint64_t product = (uint64_t)output[j] * 10ULL + carry;
      temp_output[j] = (uint32_t)product;
      carry = product >> 32;
    }
    if (carry) {
      can_continue = 0;
    } else {
      (*target_scale)++;
      for (j = 0; j < 6; j++) {
        output[j] = temp_output[j];
      }
    }
  }
}