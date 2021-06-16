#include "cursor.h"
#include "direction.h"
#include "display_utils.h"
#include "image.h"

const display_pixel_t CURSOR_COLOR = {.fields = {.r = (uint8_t)DISPLAY_MAX_RED, .g = 0, .b = 0}};

cursor_t cursor_create() {
  cursor_t cursor = {
    .x = 0,
    .y = 0,
    .shown = false,
    .shown_at = 0,
  };

  return cursor;
}

void cursor_center(cursor_t *cursor, image_region_t region) {
  cursor->x = region.x + region.width / 2;
  cursor->y = region.y + region.height / 2;
}

bool cursor_move(cursor_t *cursor, image_region_t region, direction_t direction, int16_t amount) {
  int32_t x = cursor->x, y = cursor->y;
  direction_move_xy(direction, &x, &y, amount);

  if (x < region.x) {
    x = region.x;
  } else if (x > region.x + region.width - 1) {
    x = region.x + region.width - 1;
  }

  if (y < region.y) {
    y = region.y;
  } else if (y > region.y + region.height - 1) {
    y = region.y + region.height - 1;
  }

  bool moved = cursor->x != x || cursor->y != y;
  cursor->x = x;
  cursor->y = y;
  return moved;
}

void cursor_show(cursor_t *cursor, display_t *display) {
  cursor_hide(cursor, display);
  cursor->shown_at = time(NULL);
  cursor->shown = true;

  uint16_t base_x = cursor->x;
  uint16_t base_y = cursor->y;

  uint16_t first_x = base_x - CURSOR_SIZE / 2;
  uint16_t first_y = base_y - CURSOR_SIZE / 2;

  for (int i = 0; i < CURSOR_SIZE; i++) {
    uint16_t x = first_x + i;
    uint16_t y = first_y + i;

    cursor->previous_display_data[i] = display_get_pixel(display, base_x, y);
    cursor->previous_display_data[i + CURSOR_SIZE] = display_get_pixel(display, x, base_y);

    display_set_pixel(display, base_x, y, CURSOR_COLOR);
    display_set_pixel(display, x, base_y, CURSOR_COLOR);
  }
}

void cursor_hide(cursor_t *cursor, display_t *display) {
  if (!cursor->shown) {
    return;
  }

  uint16_t base_x = cursor->x;
  uint16_t base_y = cursor->y;

  uint16_t first_x = base_x - CURSOR_SIZE / 2;
  uint16_t first_y = base_y - CURSOR_SIZE / 2;

  for (int i = 0; i < CURSOR_SIZE; i++) {
    uint16_t x = first_x + i;
    uint16_t y = first_y + i;

    display_set_pixel(display, base_x, y, cursor->previous_display_data[i]);
    display_set_pixel(display, x, base_y, cursor->previous_display_data[i + CURSOR_SIZE]);
  }

  cursor->shown = false;
}
