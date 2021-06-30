#include "gui.h"

component_t* gui_one_container_set_component(container_t *container,
                                     component_t component) {
  container->inner.one.component = component;
  container->inner.one.set = true;

  return &container->inner.one.component;
}

void gui_one_container_render(gui_t *gui, container_t *container) {
  if (container->inner.one.set) {
    gui_component_render(gui, container, &container->inner.one.component);
  }
}

void gui_one_container_update(gui_t *gui, container_t *container) {
  if (container->inner.one.set) {
    gui_component_update(gui, container, &container->inner.one.component);

    if (container->inner.one.component.focusable) {
      container->inner.one.component.focused = container->focused;
    }
  }
}

container_t gui_one_container_create(int16_t x, int16_t y) {
  container_t container = {.x = x,
                           .y = y,
                           .type = CONT_ONE,
                           .focusable = false,
                           .focused = false,
                           .inner = {.one = {
                                         .set = false,
                                     }}};
  return container;
}
