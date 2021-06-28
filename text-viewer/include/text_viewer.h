#ifndef __TEXT_VIEWER_H__
#define __TEXT_VIEWER_H__

#include "gui.h"
#include "gui_component_text_view.h"
#include "mzapo_pheripherals.h"

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

/**
 * @brief Create text viewer struct
 *
 * @param path path to file to open
 * @param pheripherals initialized pheripherals of mzapo
 * @param logger
 * @param font to display text with
 * @return text_viewer_t
 */
text_viewer_t text_viewer_create(char *path, mzapo_pheripherals_t pheripherals,
                                 logger_t *logger, font_t font);

void file_error_log(logger_t *logger, file_error_t error);

/**
 * @brief Cleans up text viewer data
 *
 * @param text_viewer Text viewer to clean up
 */
void text_viewer_destroy(text_viewer_t *text_viewer);

/**
 * @brief Load text file and parse it into memory
 *
 * @param text_viewer
 * @return file_error_t
 */
file_error_t text_viewer_load_file(text_viewer_t *text_viewer);

/**
 * @brief Start application - GUI render loop with input handling
 *
 * @param text_viewer
 */
void text_viewer_start_loop(text_viewer_t *text_viewer);

#endif // __TEXT_VIEWER_H__
