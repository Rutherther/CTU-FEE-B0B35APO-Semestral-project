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


int main(int argc, char *argv[])
{
  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waitting\n");
      /* Wait till application holding lock releases IT or exits */
      serialize_lock(0);
    }
  }

  struct termios oldstdin;
  file_set_nonblocking(STDIN_FILENO, &oldstdin);

  logger_t logger = logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  if (argc < 2) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Not enough arguments.");
    return 1;
  }


  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Initializing display...", argv[1]);
  display_t display = mzapo_create_display();
  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Display initialized...", argv[1]);

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Image %s will be loaded.", argv[1]);

  mzapo_rgb_led_t led = mzapo_create_rgb_led();
  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();

  rgb_led_set_green(&led, LED_LEFT);
  rgb_led_set_green(&led, LED_RIGHT);
  image_viewer_t viewer = image_viewer_create(argv[1], &display, &logger, ledstrip);
  rgb_led_clear(&led, LED_LEFT);
  rgb_led_clear(&led, LED_RIGHT);

  if (viewer.error != IMERR_SUCCESS) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Could not load image %d", viewer.error);
    return 1;
  }

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Loaded image of size %d %d", viewer.image.width,
              viewer.image.height);

  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
               "Displaying image...", argv[1]);

  image_viewer_display_image(&viewer);

  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
               "Starting image viewer...", argv[1]);
  image_viewer_start_loop(&viewer, mzapo_get_knobs_address());

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Cleaning up...", argv[1]);

  display_deinit(&display);
  image_viewer_destroy(&viewer);

  file_set_blocking(STDIN_FILENO, &oldstdin);

  /* Release the lock */
  serialize_unlock();

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__, "Application quit",
              argv[1]);

  ledstrip_clear(&ledstrip);
  return 0;
}
