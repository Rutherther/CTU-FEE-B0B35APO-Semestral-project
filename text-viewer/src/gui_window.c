#include "gui.h"

window_t gui_window_create(container_t *containers, uint16_t size) {
  window_t window = {
    .containers = containers,
    .containers_size = size,
    .containers_count = 0,
  };

  return window;
}

container_t *gui_window_add_container(window_t *window, container_t container) {
  if (window->containers_count >= window->containers_size) {
    return NULL;
  }

  window->containers[window->containers_count++] = container;
  return &window->containers[window->containers_count - 1];
}
