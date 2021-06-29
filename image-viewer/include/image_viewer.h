#ifndef __IMAGE_VIEWER_H__
#define __IMAGE_VIEWER_H__

#include "cursor.h"
#include "display_utils.h"
#include "image.h"
#include "image_viewer_def.h"
#include "image_viewer_handlers.h"
#include "logger.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include <stdbool.h>

/**
 * @brief Create image viewer
 *
 * @param filename file with image
 * @param display display to show image on
 * @param logger logger to log to
 * @param ledstrip ledstrip to show loading progress and position in image
 * @return image_viewer_t
 */
image_viewer_t image_viewer_create(char *filename, display_t *display,
                                   logger_t *logger, mzapo_ledstrip_t ledstrip,
                                   mzapo_rgb_led_t rgb_leds);

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

#endif // __IMAGE_VIEWER_H__
