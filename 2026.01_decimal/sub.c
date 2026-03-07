#include "utils/utils_internal.h"

int sub(decimal value_1, decimal value_2, decimal* result) {
  int for_return = 0;
  decimal neg_value_2;

  if (result == NULL || !is_valid(value_1) || !is_valid(value_2)) {
    for_return = 1;
  } else {
    negate(value_2, &neg_value_2);  // меняем знаковый бит
  }
  return for_return == 1 ? 1 : add(value_1, neg_value_2, result);
}