#include "window_browser_items.h"

char *column_names[] = {"NAME", "TYPE", "SIZE", "MODIFIED"};

bool browser_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y,
                                            display_pixel_t color) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  logger_t *logger = bstate->gui->logger;
  if (index >= bstate->current_directory->files_count) {
    logger_error(logger, __FILE__, __FUNCTION__, __LINE__, "Tried to reach item out of index");
    return false;
  }
  file_t file = bstate->current_directory->files[index];

  return table_render_item(&bstate->table, &file, renderer, bstate->font, beg_x, beg_y,
                    color);
}

bool browser_window_list_render_header(void *state, uint32_t index,
                                              renderer_t *renderer,
                                              int16_t beg_x, int16_t beg_y,
                                              display_pixel_t color) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  renderer_render_rectangle(renderer, beg_x - 3, beg_y + bstate->font->size,
                            10000, 1, color);

  uint16_t offset = beg_x;

  for (int i = 0; i < COLUMNS_COUNT; i++) {
    renderer_write_string(renderer, offset, beg_y, 0, bstate->font, column_names[i], color);
    offset += bstate->column_widths[i];
  }
  return true;
}

#define KiB 1024ULL
#define MiB KiB *KiB
#define GiB KiB *KiB *KiB
#define TiB KiB *KiB *KiB *KiB

char *browser_get_column_data(void *state, uint16_t column, char *out) {
  file_t *file = (file_t*)state;
  switch (column) {
  case 0:
    return file->name;
  case 1:
    switch (file->type) {
    case FT_FILE:
      return "FILE";
    case FT_FOLDER:
      return "DIR";
    case FT_OTHER:
      return "OTHER";
    case FT_UNKNOWN:
      return "UNKNOWN";
    }
    break;
  case 2:
    // get size
    {
      uint64_t size = file->size;
      double transformed = size;
      char *append = "B";

      if (size > TiB) {
        transformed /= TiB;
        append = "TiB";
      } else if (size > GiB) {
        transformed /= GiB;
        append = "GiB";
      } else if (size > MiB) {
        transformed /= MiB;
        append = "MiB";
      } else if (size > KiB) {
        transformed /= KiB;
        append = "KiB";
      }

      sprintf(out, "%.2f %s", transformed, append);
      return out;
    }
  case 3:
    // date modified

    strftime(out, MAX_COLUMN_CHARS, "%c", localtime(&file->modify_time));
    return out;
  }

  return NULL;
}
