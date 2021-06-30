#ifndef __TEXT_VIEWER_LOADER_H__
#define __TEXT_VIEWER_LOADER_H__

#include "mzapo_pheripherals.h"
#include "gui.h"
#include "gui_component_text_view.h"

typedef enum {
  FILER_SUCCESS,
  FILER_NOT_FOUND,
  FILER_NO_PERMISSIONS,
  FILER_FILE_CANT_OPEN,
  FILER_CANNOT_READ,
  FILER_UNKNOWN,
} file_error_t;

typedef struct {
  gui_t gui;
  char *path;

  mzapo_pheripherals_t pheripherals;
  multiline_text_t *multiline_text;
  logger_t *logger;

  bool running;

  font_t font;
} text_viewer_t;

file_error_t text_viewer_load_file(text_viewer_t *text_viewer);

#endif // __TEXT_VIEWER_LOADER_H__
