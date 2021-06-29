#ifndef __GUI_LIST_TABLE_H__
#define __GUI_LIST_TABLE_H__

#include "gui.h"
#include "file_access.h"
#include "renderer.h"
#include "font.h"
#include <stdint.h>

#define MAX_COLUMN_CHARS 2000

typedef char *(*table_get_data_fn)(void *state, uint16_t column, char *buffer);

typedef struct {
  char **columns_names;
  uint16_t *columns_widths;
  uint16_t columns_count;
  table_get_data_fn get_data;
} gui_table_t;

bool table_render_item(gui_table_t *table_state, void *state,
                       renderer_t *renderer, font_t *font, int16_t beg_x,
                       int16_t beg_y, display_pixel_t color);
char *browser_get_column_data(void *state, uint16_t column, char *out);

bool table_update_widths(gui_table_t *table, font_t *font, void *arr,
                         uint64_t item_size, uint32_t items_count);

#endif // __GUI_LIST_TABLE_H__
