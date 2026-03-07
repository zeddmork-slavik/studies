#include "utils_internal.h"

static void do_increment(decimal* value) {
  uint64_t sum = (uint64_t)(uint32_t)value->bits[0] + 1;

  value->bits[0] = (uint32_t)sum;

  uint32_t carry = (uint32_t)(sum >> 32) &
                   1U;  // а вот хочу здесь & 1 для явности намерений - чтоб не
                        // думать отличается ли что-то где-то когда-то
  if (carry) {
    sum = (uint64_t)(uint32_t)value->bits[1] + carry;
    value->bits[1] = (uint32_t)sum;
    carry = (uint32_t)(sum >> 32) & 1;

    if (carry) {
      value->bits[2] += carry;
    }
  }
}

int round_bankers(decimal* value, int next_digit, int has_more_digits) {
  int result = 0;
  int need_increment =
      (next_digit > 5) ||
      (next_digit == 5 && (GET_BIT(value->bits[0], 0) || has_more_digits));
  if (need_increment) {
    const uint32_t max_bits[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
    if ((uint32_t)value->bits[0] == max_bits[0] &&
        (uint32_t)value->bits[1] == max_bits[1] &&
        (uint32_t)value->bits[2] == max_bits[2]) {
      result = 1;
    } else {
      do_increment(value);
    }
  }

  return result;
}