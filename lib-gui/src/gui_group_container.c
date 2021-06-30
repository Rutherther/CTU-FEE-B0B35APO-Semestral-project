#include "gui.h"

component_t *gui_group_container_add_component(container_t *container,
                                       component_t component) {
  group_container_t group = container->inner.group;
  if (group.count >= group.size) {
    return false;
  }

  group.components[group.count++] = component;
  container->inner.group = group;
  return &group.components[group.count - 1];
}

void gui_group_container_render(gui_t *gui, container_t *container) {
  group_container_t group = container->inner.group;
  for (int i = 0; i < group.size; i++) {
    gui_component_render(gui, container, &group.components[i]);
  }
}

void gui_group_container_update(gui_t *gui, container_t *container) {
  group_container_t group = container->inner.group;
  for (int i = 0; i < group.size; i++) {
    gui_component_update(gui, container, &group.components[i]);
  }
  container->inner.group = group;
}

container_t gui_group_container_create(int16_t x, int16_t y, component_t *components, uint16_t components_size) {
  container_t container = {.x = x,
                           .y = y,
                           .type = CONT_GROUP,
                           .focusable = false,
                           .focused = false,
                           .inner = {.group = {.components = components,
                                               .size = components_size,
                                               .count = 0}}};
  return container;
}
