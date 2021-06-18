#include "cursor.h"
#include "direction.h"
#include "display_utils.h"
#include "image.h"
#include "coords.h"

const uint8_t CURSOR[] = {
  0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 1st line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 2nd line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 3rd line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 4th line
  0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, // end of 5th line
  0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, // end of 6th line
  0x2, 0x2, 0x2, 0x2, 0x2, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2, // end of 7th line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 8th line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 9th line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 10th line
  0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, // end of 11th line
  0x0, 0x0
};

const display_pixel_t CURSOR_OUTLINE_COLOR = {.fields = {.r = (uint8_t)DISPLAY_MAX_RED,
                                                 .g = DISPLAY_MAX_GREEN,
                                                 .b = DISPLAY_MAX_BLUE}};
const display_pixel_t CURSOR_COLOR = {
    .fields = {.r = 0, .g = 0, .b = 0}};

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

void cursor_show(cursor_t *cursor, image_t *image, image_zoom_t zoom,
                 display_t *display) {
  cursor_hide(cursor, image, zoom, display);
  cursor->shown_at = time(NULL);
  cursor->shown = true;

  coords_t screen_coords =
      image_get_screen_coords(image, zoom, coords_create(cursor->x, cursor->y));
  uint16_t base_x = screen_coords.x;
  uint16_t base_y = screen_coords.y;

  uint16_t first_x = base_x - CURSOR_WIDTH / 2;
  uint16_t first_y = base_y - CURSOR_WIDTH / 2;

  for (uint16_t iy = 0; iy < CURSOR_WIDTH; iy++) {
    for (uint16_t ix = 0; ix < CURSOR_WIDTH; ix++) {
      uint16_t x = first_x + ix;
      uint16_t y = first_y + iy;

      uint8_t colorn = CURSOR[iy * CURSOR_WIDTH + ix];
      display_pixel_t color;
      switch (colorn) {
      case 0x1:
        color = CURSOR_COLOR;
        break;
      case 0x2:
        color = CURSOR_OUTLINE_COLOR;
        break;
      default:
        continue;
      }

      cursor->previous_display_data[iy * CURSOR_WIDTH + ix] = display_get_pixel(display, x, y);
      display_set_pixel(display, x, y, color);
    }
  }
}

void cursor_hide(cursor_t *cursor, image_t *image, image_zoom_t zoom,
                 display_t *display) {
  if (!cursor->shown) {
    return;
  }

  coords_t screen_coords =
      image_get_screen_coords(image, zoom, coords_create(cursor->x, cursor->y));
  uint16_t base_x = screen_coords.x;
  uint16_t base_y = screen_coords.y;
  uint16_t first_x = base_x - CURSOR_WIDTH / 2;
  uint16_t first_y = base_y - CURSOR_WIDTH / 2;

  for (int iy = 0; iy < CURSOR_WIDTH; iy++) {
    for (int ix = 0; ix < CURSOR_WIDTH; ix++) {
      uint16_t x = first_x + ix;
      uint16_t y = first_y + iy;

      uint8_t colorn = CURSOR[iy * CURSOR_WIDTH + ix];
      switch (colorn) {
      case 0x1:
      case 0x2:
        break;
      default:
        continue;
      }

      display_set_pixel(display, x, y,
                        cursor->previous_display_data[iy * CURSOR_WIDTH + ix]);
    }
  }

  cursor->shown = false;
}
