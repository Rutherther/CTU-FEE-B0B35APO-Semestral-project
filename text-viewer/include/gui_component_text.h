#ifndef __GUI_COMPONENT_TEXT_H__
#define __GUI_COMPONENT_TEXT_H__

#include "display_utils.h"
#include "gui.h"

typedef struct {
  char *line;
  font_t *font;
  display_pixel_t color;
} text_t;

component_t gui_text_create(text_t *text, int16_t x, int16_t y,
                            int16_t w, int16_t h);

void gui_text_render(container_t *container, component_t *component,
                     gui_t *gui);
void gui_text_update(container_t *container, component_t *component,
                     gui_t *gui);

#endif // __GUI_COMPONENT_TEXT_H__
