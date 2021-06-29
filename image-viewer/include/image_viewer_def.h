#ifndef __IMAGE_VIEWER_DEF_H__
#define __IMAGE_VIEWER_DEF_H__

#include "image.h"
#include "cursor.h"
#include "mzapo_pheripherals.h"

typedef enum {
  MOD_CURSOR,
  MOD_IMAGE,
  MOD_COUNT,
} image_viewer_mode_t;

typedef struct {
  cursor_t cursor;

  image_t image;

  image_zoom_t scale;
  image_region_t image_region;
  image_region_t display_region;

  image_viewer_mode_t mode;

  display_t *display;
  bool running;

  mzapo_ledstrip_t ledstrip;
  mzapo_rgb_led_t rgb_leds;

  logger_t *logger;
  image_error_t error;

  bool should_render;
} image_viewer_t;

#endif // __IMAGE_VIEWER_DEF_H__
