#include "font.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include "nonblocking_io.h"
#include "serialize_lock.h"
#include <stdio.h>
#include <unistd.h>
#include "logger.h"
#include "display_utils.h"
#include "mzapo_pheripherals.h"
#include "text_viewer.h"

#ifdef COMPUTER
#include "mzapo_sdl.h"
#endif

typedef enum {
  ERROR_SUCCESS,
  ERROR_NOT_ENOUGH_ARGUMENTS,
  ERROR_CANT_OPEN_FILE,
  ERROR_PHERIPHERALS,
} error_t;

int main(int argc, char *argv[]) {
  #ifdef COMPUTER
  mzapo_sdl_init();
  #endif
  struct termios oldstdin;

  logger_t logger =
      logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");
    logger_warn(&logger, __FILE__, __FUNCTION__, __LINE__, "System is occupied");

    if (1) {
      /* Wait till application holding lock releases IT or exits */
      logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
                  "Waiting");
      serialize_lock(0);
    }
  }

  mzapo_rgb_led_t rgb_leds = mzapo_create_rgb_led();
  rgb_led_clear(&rgb_leds, LED_LEFT);
  rgb_led_clear(&rgb_leds, LED_RIGHT);

  if (argc < 2) {
    logger_info(&logger, __FILE__, __FUNCTION__, __LINE__, "Not enough arguments");
    rgb_led_set_red(&rgb_leds, LED_LEFT);
    return ERROR_NOT_ENOUGH_ARGUMENTS;
  }

  display_t display = mzapo_create_display();
  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();
  void* knobs = mzapo_get_knobs_address();

  if (!mzapo_check_pheripherals(&ledstrip, &rgb_leds, &display, &knobs)) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Could not initialize some of the pheripherals.");
    rgb_led_set_red(&rgb_leds, LED_LEFT);
    return ERROR_PHERIPHERALS;
  }

  mzapo_pheripherals_t pheripherals = mzapo_pheripherals_create(&ledstrip, &rgb_leds, &display, &knobs);

  font_t font = font_family_create(font_wTahoma_22, &fontFamily_wTahoma);
  font.size = 20;
  font.char_spacing = 2;

  text_viewer_t text_viewer = text_viewer_create(argv[1], pheripherals, &logger, font);

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Loading file");
  file_error_t error = text_viewer_load_file(&text_viewer);
  if (error != FILER_SUCCESS) {
    file_error_log(&logger, error);
    rgb_led_set_red(&rgb_leds, LED_RIGHT);
    return ERROR_CANT_OPEN_FILE;
  }

  file_set_nonblocking(STDIN_FILENO, &oldstdin);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Starting text viewer");
  text_viewer_start_loop(&text_viewer);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Closing application");

  text_viewer_destroy(&text_viewer);
  display_deinit(&display);

  file_set_blocking(STDIN_FILENO, &oldstdin);

  serialize_unlock();
  
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Application quit");

#ifdef COMPUTER
  mzapo_sdl_deinit();
#endif
  return ERROR_SUCCESS;
}
