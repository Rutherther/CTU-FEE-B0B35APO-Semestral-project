#ifndef __WINDOW_BROWSER_ITEMS_H__
#define __WINDOW_BROWSER_ITEMS_H__

#include "gui_component_text.h"
#include "gui_list_table.h"
#include <stdint.h>
#include "renderer.h"
#include "display_utils.h"
#include "gui_list_commands.h"
#include "file_access.h"

#define COLUMNS_COUNT 4
typedef struct {
  bool running;
  gui_t *gui;

  container_t *list_container;
  component_t *line_component;
  window_t *browser_window;

  font_t *font;

  gui_list_command_state_t click_state;
  text_t text_state;

  directory_t *current_directory;
  fileaccess_state_t state;

  gui_table_t table;
  uint16_t column_widths[COLUMNS_COUNT];
} browser_window_state_t;

extern char *column_names[];

bool browser_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y,
                                            display_pixel_t color);

bool browser_window_list_render_header(void *state, uint32_t index,
                                              renderer_t *renderer,
                                              int16_t beg_x, int16_t beg_y,
                                              display_pixel_t color);

char *browser_get_column_data(void *state, uint16_t column, char *out);
#endif // __WINDOW_BROWSER_ITEMS_H__
