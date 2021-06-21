#ifndef __CURSOR_H__
#define __CURSOR_H__

#include "display_utils.h"
#include "image.h"
#include "direction.h"
#include <time.h>
#include <stdint.h>

#define CURSOR_WIDTH 11

typedef struct {
  uint16_t x;
  uint16_t y;

  bool shown;
  time_t shown_at;

  display_pixel_t previous_display_data[CURSOR_WIDTH * CURSOR_WIDTH];
} cursor_t;

extern const display_pixel_t CURSOR_COLOR;
extern const display_pixel_t CURSOR_OUTLINE_COLOR;

/**
 * @brief Create cursor
 * 
 * @return cursor_t 
 */
cursor_t cursor_create();

/**
 * @brief Center cursor within region
 * 
 * @param cursor 
 * @param region within to center the cursor
 */
void cursor_center(cursor_t *cursor, image_region_t region);

/**
 * @brief Move cursor in given direction within region given
 * 
 * @param cursor 
 * @param region to move cursor within, if cursor is outside, move it inside
 * @param direction what direction to move cursor to
 * @param amount how many steps to move
 * @return true cursor was moved
 * @return false cursor was not moved
 */
bool cursor_move(cursor_t *cursor, image_region_t region, direction_t direction, int16_t amount);

/**
 * @brief Show cursor on display screen
 * 
 * @param cursor 
 * @param image image to map image coords to screen coords
 * @param zoom zoom to map image coords to screen coords
 * @param display 
 */
void cursor_show(cursor_t *cursor, image_t *image, image_zoom_t zoom, display_t *display);

/**
 * @brief Hide cursor shown on display
 * 
 * @param cursor 
 * @param image image to map image coords to screen coords
 * @param zoom zoom to map image coords to screen coords
 * @param display 
 */
void cursor_hide(cursor_t *cursor, image_t *image, image_zoom_t zoom, display_t *display);

#endif // __CURSOR_H__
