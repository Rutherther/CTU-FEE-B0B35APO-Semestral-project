#include "direction.h"

void direction_move_xy(direction_t direction, uint16_t *x, uint16_t *y, int16_t amount) {
  switch (direction) {
  case LEFT:
    *x -= amount;
    break;
  case RIGHT:
    *x += amount;
    break;
  case UP:
    *y -= amount;
    break;
  case DOWN:
    *y += amount;
    break;
  }
}
