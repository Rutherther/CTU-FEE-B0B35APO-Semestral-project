#ifndef __IMAGE_VIEWER_H__
#define __IMAGE_VIEWER_H__

#include <stdbool.h>
#include "image.h"
#include "display_utils.h"
#include "cursor.h"
#include "logger.h"
#include "mzapo_led_strip.h"

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

  logger_t *logger;
  image_error_t error;
} image_viewer_t;

/**
 * @brief Create image viewer
 * 
 * @param filename file with image
 * @param display display to show image on
 * @param logger logger to log to
 * @param ledstrip ledstrip to show loading progress and position in image
 * @return image_viewer_t 
 */
image_viewer_t image_viewer_create(char *filename, display_t *display, logger_t *logger, mzapo_ledstrip_t ledstrip);

/**
 * @brief Clean up image viewer data
 * 
 * @param viewer 
 */
void image_viewer_destroy(image_viewer_t *viewer);

/**
 * @brief Start render loop with handling input
 * 
 * @param viewer 
 * @param reg_knobs_base base address of reg knobs 
 */
void image_viewer_start_loop(image_viewer_t *viewer, void *reg_knobs_base);

/**
 * @brief Display image on display
 * 
 * @param viewer 
 */
void image_viewer_display_image(image_viewer_t *viewer);

#endif // __IMAGE_VIEWER_H__
