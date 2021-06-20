#include "gui.h"
#include "input.h"
#include "logger.h"
#include "renderer.h"
#include <jpeglib.h>

gui_t gui_create(logger_t *logger, commands_t *commands, renderer_t *renderer,
                 mzapo_pheripherals_t *pheripherals) {
  gui_t gui = {
      .pheripherals = pheripherals,
      .renderer = renderer,
      .active_window = NULL,
      .commands = commands,
      .logger = logger,
      .size = {.x = DISPLAY_WIDTH, .y = DISPLAY_HEIGHT},
  };

  return gui;
}

void gui_render(gui_t *gui) {
  renderer_clear(gui->renderer);
  window_t *window = gui->active_window;
  if (window == NULL) {
    logger_warn(gui->logger, __FILE__, __FUNCTION__, __LINE__,
                "No active window");
    return;
  }

  for (int i = 0; i < window->containers_count; i++) {
    gui_container_render(gui, &window->containers[i]);
  }

  renderer_render(gui->renderer);
}

component_t *gui_one_container_get_component(container_t *container) {
  return &container->inner.one.component;
}

void gui_update(gui_t *gui) {
  window_t *window = gui->active_window;
  if (window == NULL) {
    logger_warn(gui->logger, __FILE__, __FUNCTION__, __LINE__,
                "No active window");
    return;
  }

  for (int i = 0; i < window->containers_count; i++) {
    gui_container_update(gui, &window->containers[i]);
  }
}

void gui_set_active_window(gui_t *gui, window_t *window) {
  gui->active_window = window;
}
