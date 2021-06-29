#include <stdlib.h>
#include <stdio.h>

#include "file_access.h"
#include "file_browser.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include "options.h"
#include "font.h"
#include "logger.h"
#include "mzapo_pheripherals.h"
#include "nonblocking_io.h"
#include "serialize_lock.h"

#ifdef COMPUTER
#include "mzapo_sdl.h"
#endif

// TODO: okna
//DONE   initial - lokální fs nebo nějaký unmounted
//DONE   browser - ukazuje current složku, v ní soubory a info o nich, nahoře název
//DONE     složky
//TODO   contextmenu - ukazuje info o souboru a jeho nabídku na smazání, přesunutí,
//     zkopírování, zobrazení v textovém move/copy - slouží podobně jako initial a
//     browser, ale k přesunutí nebo
//     zkopírování objektu
//DONE   dialog

// TODO: dokončit dokumentaci

// TODO: controls - textový soubor
// TODO: lepší controls v prohlížeči textu

// periferie v průzkumníku 10:30 - 11:30
// učesat kód 11 - 14 (15)
// controls 12
// context menu 15 - 16

// DEADLINE kód 16:00
// DEADLINE dokumentace 23:00
// dokončit dokumentaci a vygenerovat doxygen - úterý

typedef enum {
  ERROR_SUCCESS,
  ERROR_PHERIPHERALS,
  ERROR_OPTIONS,
} error_t;

#define OPTIONS_PATH "eoptions.cfg"

static void exec_options_create_default() {
  exec_option_t options_arr[] = {
      {.mime = "image/png", .program = "./bin/image-viewer"},
      {.mime = "image/jpeg", .program = "./bin/image-viewer"},
      {.mime = "image/x-portable-pixmap", .program = "./bin/image-viewer"},
      {.mime = "image/x-portable-anymap", .program = "./bin/image-viewer"},
      {.mime = "text/plain", .program = "./bin/text-viewer"},
      {.mime = "text", .program = "./bin/text-viewer"},
  };
  exec_options_t options = {
    .options_count = sizeof(options_arr) / sizeof(options_arr[0]),
    .options = options_arr,
  };

  exec_options_save(&options, OPTIONS_PATH);
}

static file_operation_error_t exec_options_init(exec_options_loader_t *loader, char *exec_buffer, logger_t *logger, file_operation_error_t error) {
  if (error == FILOPER_SUCCESS) {
    error = exec_options_loader_load(loader, exec_buffer);
  }

  if (error == FILOPER_DOES_NOT_EXIST) {
    exec_options_create_default();
  }

  if (error != FILOPER_SUCCESS) {
    return error;
  }
  browser_exec_options = loader->exec_options;
  return error;
}

static error_t file_browser_start(logger_t *logger) {
  mzapo_rgb_led_t rgb_leds = mzapo_create_rgb_led();

  display_t display = mzapo_create_display();
  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();
  void *knobs = mzapo_get_knobs_address();
  struct termios oldstdin;
  if (!mzapo_check_pheripherals(&ledstrip, &rgb_leds, &display, &knobs)) {
    logger_error(logger, __FILE__, __FUNCTION__, __LINE__,
                 "Could not initialize some of the pheripherals.");
    rgb_led_set_red(&rgb_leds, LED_LEFT);
    return ERROR_PHERIPHERALS;
  }

  mzapo_pheripherals_t pheripherals =
      mzapo_pheripherals_create(&ledstrip, &rgb_leds, &display, &knobs);
  mzapo_pheripherals_clear(&pheripherals);

  font_t font = font_family_create(font_wTahoma_22, &fontFamily_wTahoma);
  font.char_spacing = 2;

  file_browser_t file_browser =
      file_browser_create(pheripherals, logger, font);

  file_set_nonblocking(STDIN_FILENO, &oldstdin);
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Starting file browser");
  file_browser_start_loop(&file_browser);
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Closing application");

  file_browser_destroy(&file_browser);
  mzapo_pheripherals_clear(&pheripherals);
  file_set_blocking(STDIN_FILENO, &oldstdin);

  return ERROR_SUCCESS;
}

int main(int argc, char *argv[]) {
  #ifdef COMPUTER
  mzapo_sdl_init();
  #endif

  logger_t logger =
      logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  exec_options_loader_t loader = exec_options_loader_create(OPTIONS_PATH);
  file_operation_error_t error = exec_options_loader_get_size(&loader);
  char exec_buffer[loader.bytes_size];

  error = exec_options_init(&loader, exec_buffer, &logger, error);
  if (error != FILOPER_SUCCESS) {
    fileaccess_log_error(&logger, error);
    return ERROR_OPTIONS;
  }

  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    logger_warn(&logger, __FILE__, __FUNCTION__, __LINE__, "System is occupied");
    logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
                "Waiting");
    serialize_lock(0);
  }

  error_t rerror = file_browser_start(&logger);

  serialize_unlock();

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Application quit");

#ifdef COMPUTER
  mzapo_sdl_deinit();
#endif
  return rerror;
}
