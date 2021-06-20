#include "gui_component_text.h"
#include "font.h"
#include "renderer.h"

component_t gui_text_create(text_t *text, int16_t x, int16_t y, int16_t w,
                            int16_t h) {
  component_t comp =
      gui_component_create(x, y, w, h, gui_text_render, gui_text_update);
  comp.state = text;

  if (w == 0 || h == 0) {
    size2d_t measured = font_measure_text(text->font, text->line);

    comp.width = comp.width != 0 ? comp.width : measured.x;
    comp.height = comp.height != 0 ? comp.height : measured.y;
  }

  return comp;
}

void gui_text_render(container_t *container, component_t *component,
                     gui_t *gui) {
  if (gui_is_component_visible(gui, container, component)) {
    text_t *state = (text_t*)component->state;
    renderer_write_string(gui->renderer, component->x, component->y, 0, state->font, state->line, state->color);
  }
}

void gui_text_update(container_t *container, component_t *component,
                     gui_t *gui) {
  // do nothing -- maybe horizontal scroll?
}
