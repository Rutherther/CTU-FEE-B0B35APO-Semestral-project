#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include <stdint.h>

typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT,
} direction_t;

void direction_move_xy(direction_t direction, int32_t *x, int32_t *y, int16_t amount);

#endif // __DIRECTION_H__
