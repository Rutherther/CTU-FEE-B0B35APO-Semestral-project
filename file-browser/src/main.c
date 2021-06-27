#include <stdlib.h>
#include <stdio.h>

#include "file_browser.h"
#include "logger.h"
#include "serialize_lock.h"
#include "mzapo_sdl.h"
#include "mzapo_pheripherals.h"
#include "nonblocking_io.h"
#include "font.h"

// soubory otvírat podle mime typů - config s mime typy a přiřazeným editorem
// pokud nic nevyhovuje a je spustitelný, tak spustit
// pokud neplatí ani jedno, otevřít v text vieweru - ten schovat pod mime type text, který bude default

// TODO: okna
//   initial - lokální fs nebo nějaký unmounted
//   browser - ukazuje current složku, v ní soubory a info o nich, nahoře název
//     složky
//   contextmenu - ukazuje info o souboru a jeho nabídku na smazání, přesunutí,
//     zkopírování, zobrazení v textovém move/copy - slouží podobně jako initial a
//     browser, ale k přesunutí nebo
//     zkopírování objektu
//   options - nastavení nabídka
//     mimetypes - jaký mime type otevírá co, defaultně dát jen pár mime types s možností přidat přes stdin (příp. virtuální klávesnici, když zbude čas)

// TODO: dokončit dokumentaci

// options, browser, contextmenu
// spouštění souborů // 15 - 16

// dokončit dokumentaci a vygenerovat doxygen - 2 HOD 19 - 21
// pátek - lazení
// sobota, neděle - zoomování fontu

typedef enum {
  ERROR_SUCCESS,
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

  display_t display = mzapo_create_display();
  mzapo_ledstrip_t ledstrip = mzapo_create_ledstrip();
  void* knobs = mzapo_get_knobs_address();

  // TODO: check permissions for mounted devices

  if (!mzapo_check_pheripherals(&ledstrip, &rgb_leds, &display, &knobs)) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Could not initialize some of the pheripherals.");
    rgb_led_set_red(&rgb_leds, LED_LEFT);
    return ERROR_PHERIPHERALS;
  }

  mzapo_pheripherals_t pheripherals = mzapo_pheripherals_create(&ledstrip, &rgb_leds, &display, &knobs);

  font_t font = font_create(font_rom8x16);
  font.char_spacing = 2;

  file_browser_t file_browser = file_browser_create(pheripherals, &logger, font);

  file_set_nonblocking(STDIN_FILENO, &oldstdin);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Starting file browser");
  file_browser_start_loop(&file_browser);
  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Closing application");

  file_browser_destroy(&file_browser);
  display_deinit(&display);

  file_set_blocking(STDIN_FILENO, &oldstdin);

  serialize_unlock();

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Application quit");

#ifdef COMPUTER
  mzapo_sdl_deinit();
#endif
  return ERROR_SUCCESS;

  return 0;
}
