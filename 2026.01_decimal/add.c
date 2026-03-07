#include "utils/utils_internal.h"

int add(decimal value_1, decimal value_2, decimal* result) {
  int for_return = 0;

  if (result == NULL) {
    for_return = 1;
  } else {
    int sign1 = get_sign(value_1);
    int sign2 = get_sign(value_2);

    set_sign(&value_1, 0);
    set_sign(&value_2, 0);
    normalize(&value_1);
    normalize(&value_2);

    int scale1 = get_scale(value_1);
    int scale2 = get_scale(value_2);
    uint32_t big1[6] = {0};
    uint32_t big2[6] = {0};
    uint32_t mantissa1[3], mantissa2[3];
    get_mantissa_as_array(value_1, mantissa1);
    get_mantissa_as_array(value_2, mantissa2);

    for (int i = 0; i < 3; i++) {
      big1[i] = mantissa1[i];
      big2[i] = mantissa2[i];
    }
    int target_scale = (scale1 > scale2) ? scale1 : scale2;
    increase_scale_to_target(big1, scale1, target_scale);
    increase_scale_to_target(big2, scale2, target_scale);

    uint32_t big_result[6] = {0};
    int result_sign = 0;
    int op_type = determine_operation_type(sign1, sign2);
    perform_192bit_operation(big1, big2, op_type, big_result, &result_sign);
    for_return = change_scale_with_rounding(big_result, target_scale,
                                            result_sign, result);
  }
  return for_return;
}