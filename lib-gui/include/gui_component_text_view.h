#ifndef __GUI_COMPONENT_TEXT_VIEW_H__
#define __GUI_COMPONENT_TEXT_VIEW_H__

#include "display_utils.h"
#include "gui.h"

typedef struct {
  char *line;
  uint16_t length;
} line_t;

typedef struct {
  line_t *lines;
  uint16_t lines_count;
  char *text;
  font_t *font;
  display_pixel_t color;
} multiline_text_t;

multiline_text_t *gui_multiline_text_create(font_t *font, display_pixel_t color,
                                            char *text);

component_t gui_text_view_create(gui_t *gui, multiline_text_t *text, int16_t x,
                                 int16_t y);

uint16_t gui_text_view_get_lines_scrolled(component_t *component);

void gui_text_view_scroll(component_t *text_view, int32_t x, int32_t y);
void gui_text_view_reset_scroll(component_t *text_view);
void gui_text_view_full_scroll(component_t *text_view);

void gui_text_view_render(container_t *container, component_t *component,
                          gui_t *gui);
void gui_text_view_update(container_t *container, component_t *component,
                          gui_t *gui);

#endif // __GUI_COMPONENT_TEXT_VIEW_H__
