#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include <stdint.h>

typedef enum {
  UP,
  LEFT,
  DOWN,
  RIGHT,
} direction_t;

void direction_move_xy(direction_t direction, uint16_t *x, uint16_t *y, int16_t amount);

#endif // __DIRECTION_H__
