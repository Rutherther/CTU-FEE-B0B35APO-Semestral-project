/*******************************************************************
  Image viewer (png, jpeg, ppm)
  made by František Boháček
 *******************************************************************/

#include "display_utils.h"
#include "image.h"
#include "image_viewer.h"

#include <png.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "mzapo_led_strip.h"
#include "mzapo_pheripherals.h"
#include "nonblocking_io.h"
#include "logger.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "mzapo_rgb_led.h"

#ifdef COMPUTER
#include "mzapo_sdl.h"
#endif

typedef enum {
  ERROR_SUCCESS,
  ERROR_TOO_FEW_ARGUMENTS,
  ERROR_IMAGE_ERROR
} error_t;

error_t image_viewer_start(logger_t *logger, char *file_name, mzapo_rgb_led_t led) {
  struct termios oldstdin;

  display_t display = mzapo_create_display();

  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Image %s will be loaded.", file_name);

  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();

  rgb_led_set_green(&led, LED_LEFT);
  rgb_led_set_green(&led, LED_RIGHT);
  image_viewer_t viewer = image_viewer_create(file_name, &display, logger, ledstrip, led);
  rgb_led_clear(&led, LED_LEFT);
  rgb_led_clear(&led, LED_RIGHT);

  if (viewer.error != IMERR_SUCCESS) {
    image_error_log(logger, viewer.error);
    rgb_led_set_red(&led, LED_RIGHT);
    return ERROR_IMAGE_ERROR;
  }

  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Loaded image of size %d %d", viewer.image.width,
              viewer.image.height);

  logger_debug(logger, __FILE__, __FUNCTION__, __LINE__,
               "Displaying image...", file_name);

  image_viewer_display_image(&viewer);

  logger_debug(logger, __FILE__, __FUNCTION__, __LINE__,
               "Starting image viewer...", file_name);
  file_set_nonblocking(STDIN_FILENO, &oldstdin);
  image_viewer_start_loop(&viewer, mzapo_get_knobs_address());

  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Cleaning up...", file_name);

  display_deinit(&display);
  image_viewer_destroy(&viewer);

  file_set_blocking(STDIN_FILENO, &oldstdin);
  ledstrip_clear(&ledstrip);
  return ERROR_SUCCESS;
}

int main(int argc, char *argv[])
{
#ifdef COMPUTER
  mzapo_sdl_init();
#endif

  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waitting\n");
      /* Wait till application holding lock releases IT or exits */
      serialize_lock(0);
    }
  }

  mzapo_rgb_led_t led = mzapo_create_rgb_led();
  logger_t logger = logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  if (argc < 2) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Not enough arguments.");
    rgb_led_set_red(&led, LED_LEFT);
    return ERROR_TOO_FEW_ARGUMENTS;
  }

  error_t error = image_viewer_start(&logger, argv[1], led);
  /* Release the lock */
  serialize_unlock();

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__, "Application quit",
              argv[1]);


#ifdef COMPUTER
  mzapo_sdl_deinit();
#endif
  return error;
}
