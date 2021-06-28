#include "gui_component_text.h"
#include "font.h"
#include "renderer.h"
#include <string.h>

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
    text_t *state = (text_t *)component->state;
    uint16_t line_height = state->font->size + state->font->line_spacing;
    uint16_t lines_fit = component->height / line_height;

    if (lines_fit == 0) {
      lines_fit = 1;
    }

    int32_t remaining = strlen(state->line);
    const char *line = state->line;

    int16_t y = component->y;

    while (lines_fit > 0 && remaining > 0) {
      lines_fit--;

      size2d_t size = {.x = component->width, .y = line_height};
      uint16_t fit_chars = font_fit_cut(state->font, size, line);
      if (fit_chars == 0) {
        fit_chars = remaining;
      }

      for (int i = 0; i < fit_chars; i++) {
        if (line[i] == '\n' && lines_fit > 1) {
          fit_chars = i + 1;
          break;
        }
      }

      renderer_write_string(gui->renderer, component->x, y, fit_chars,
                            state->font, line, state->color);

      y += line_height;
      remaining -= fit_chars;
      line += fit_chars;
    }

  }
}

void gui_text_update(container_t *container, component_t *component,
                     gui_t *gui) {
  // do nothing -- maybe horizontal scroll?
}
