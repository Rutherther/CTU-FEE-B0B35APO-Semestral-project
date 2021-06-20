#include "gui_component_line.h"
#include "display_utils.h"
#include "gui.h"
#include "renderer.h"

component_t gui_line_create(const display_pixel_t *color, int16_t x, int16_t y,
                            int16_t w, int16_t h) {
  component_t line = gui_component_create(x, y, w, h, gui_line_render, gui_line_update);
  line.state = (void*)color;

  return line;
}

void gui_line_render(container_t *container, component_t *component,
                     gui_t *gui) {
  if (gui_is_component_visible(gui, container, component)) {
    coords_t coords = gui_component_get_screen_position(container, component);
    renderer_render_border(gui->renderer, coords.x, coords.y, component->width,
                           component->height,
                           *(display_pixel_t *)component->state);
  }
}

void gui_line_update(container_t *container, component_t *component,
                     gui_t *gui) {
  // do nothing
}
