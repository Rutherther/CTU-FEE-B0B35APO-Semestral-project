#include "gui.h"

component_t gui_component_create(int16_t x, int16_t y, uint16_t w, uint16_t h,
                                 render_function render,
                                 update_function update) {
  component_t component = {
    .x = x,
    .y = y,
    .width = w,
    .height = h,
    .render = render,
    .update = update,
    .focused = false,
    .focusable = false,
    .state = NULL
  };

  return component;
}

bool gui_is_component_visible(gui_t *gui, container_t *container,
                              component_t *component) {
  coords_t pos = gui_component_get_screen_position(container, component);

  bool visible_x = (pos.x >= 0 && pos.x < gui->size.x) ||
                   (pos.x < 0 && pos.x + component->width > 0);
  bool visible_y = (pos.y >= 0 && pos.y < gui->size.y) ||
                   (pos.y < 0 && pos.y + component->height > 0);

  return visible_x && visible_y;
}

void gui_component_render(gui_t *gui, container_t *container,
                          component_t *component) {
  component->render(container, component, gui);
}

void gui_component_update(gui_t *gui, container_t *container,
                          component_t *component) {
  component->update(container, component, gui);
}

coords_t gui_component_get_absolute_position(container_t *container,
                                             component_t *component) {
  coords_t pos = {
    .x = container->x + component->x,
    .y = container->y + component->y
  };

  return pos;
}

coords_t gui_component_get_screen_position(container_t *container,
                                          component_t *component) {
  return gui_component_get_absolute_position(container, component);
}
