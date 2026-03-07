#ifndef DECIMAL_H
#define DECIMAL_H

#include <stdlib.h>

#define GET_BIT(x, n) (((x) >> (n)) & 1U)
#define SET_BIT(x, n) ((x) |= (1U << (n)))

typedef struct {
  int bits[4];
} decimal;

#endif