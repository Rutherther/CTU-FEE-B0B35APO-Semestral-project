#include "file_browser.h"
#include "gui.h"
#include "input.h"
#include "logger.h"
#include "renderer.h"
#include "window_initial.h"

file_browser_t file_browser_create(mzapo_pheripherals_t pheripherals,
                                   logger_t *logger, font_t font) {
  file_browser_t browser = {
    .font = font,
    .logger = logger,
    .pheripherals = pheripherals,
  };

  return browser;
}

void file_browser_start_loop(file_browser_t *file_browser) {
  command_t command_arr[100];
  commands_t commands = commands_create(command_arr, 100, *file_browser->pheripherals.knobs);
  renderer_t renderer = renderer_create(file_browser->pheripherals.display);

  file_browser->gui = gui_create(file_browser->logger, &commands, &renderer, &file_browser->pheripherals);
  window_initial_open(&file_browser->gui, file_browser->font);
  logger_info(file_browser->logger, __FILE__, __FUNCTION__, __LINE__, "File browser loop ended");
}

void file_browser_destroy(file_browser_t *file_browser) {
  // do nothing
}
