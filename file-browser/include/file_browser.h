#ifndef __FILE_BROWSER_H__
#define __FILE_BROWSER_H__

#include "logger.h"
#include "gui.h"
#include "font.h"

typedef struct {
  logger_t *logger;

  mzapo_pheripherals_t pheripherals;

  gui_t gui;
  font_t font;
} file_browser_t;

file_browser_t file_browser_create(mzapo_pheripherals_t pheripherals, logger_t *logger, font_t font);
void file_browser_start_loop(file_browser_t *file_browser);
void file_browser_destroy(file_browser_t *file_browser);

#endif // __FILE_BROWSER_H__
