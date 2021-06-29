#include "gui_list_table.h"

bool table_render_item(gui_table_t *table_state, void *state,
                       renderer_t *renderer, font_t *font, int16_t beg_x,
                       int16_t beg_y, display_pixel_t color) {
  uint16_t offset = beg_x;
  char tmp[MAX_COLUMN_CHARS];
  for (int i = 0; i < table_state->columns_count; i++) {
    char *data = table_state->get_data(state, i, tmp);
    renderer_write_string(renderer, offset, beg_y, 0, font, data,
                          color);
    offset += table_state->columns_widths[i];
  }
  return true;
}

bool table_update_widths(gui_table_t *table, font_t *font, void *arr, size_t item_size,
                         uint32_t items_count) {
  char tmp[MAX_COLUMN_CHARS];
  for (int i = 0; i < table->columns_count; i++) {
    uint16_t max_size = font_measure_text(font, table->columns_names[i]).x;
    for (int j = 0; j < items_count; j++) {
      char *data =
          browser_get_column_data(arr + item_size * i, i, tmp);
      if (data == NULL) {
        continue;
      }
      uint16_t current_size = font_measure_text(font, data).x;

      if (current_size > max_size) {
        max_size = current_size;
      }
    }
    table->columns_widths[i] = max_size + 50;
  }
  return true;
}
