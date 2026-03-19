#include "utils_internal.h"

int compare_192bit_with_max_decimal(const uint32_t a[6]) {
  const uint32_t max_decimal[6] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0};

  int for_return = 0;
  int continue_comparing = 1;
  int i;

  i = 5;
  while (i >= 3 && continue_comparing) {
    if (a[i] > 0) {
      for_return = 1;
      continue_comparing = 0;
    }
    i--;
  }

  while (i >= 0 && continue_comparing) {
    if (a[i] > max_decimal[i]) {
      for_return = 1;
      continue_comparing = 0;
    } else if (a[i] < max_decimal[i]) {
      for_return = 0;
      continue_comparing = 0;
    }
    i--;
  }

  return for_return;
}