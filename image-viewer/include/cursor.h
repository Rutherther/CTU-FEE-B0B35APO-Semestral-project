#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "display_utils.h"
#include "image.h"
#include "direction.h"
#include <time.h>
#include <stdint.h>

#define CURSOR_SIZE 10

typedef struct {
  uint16_t x;
  uint16_t y;

  bool shown;
  time_t shown_at;

  display_pixel_t previous_display_data[CURSOR_SIZE * 2 - 1];
} cursor_t;

extern const display_pixel_t CURSOR_COLOR;

cursor_t cursor_create();
void cursor_center(cursor_t *cursor, image_region_t region);
bool cursor_move(cursor_t *cursor, image_region_t region, direction_t direction, int16_t amount);

void cursor_show(cursor_t *cursor, display_t *display);
void cursor_hide(cursor_t *cursor, display_t *display);

#endif // __CURSOR_H__
