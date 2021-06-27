#include "gui_window_info.h"
#include "gui.h"
#include "gui_container_info.h"

bool gui_window_init_and_loop(gui_t *gui, void *state, window_info_t info,
                              gui_loop_running_fn loop_running,
                              gui_loop_job_fn job) {
  container_t containers[info.containers_count];
  window_t window = gui_window_create(containers, info.containers_count);

  uint32_t components_count = 0;
  for (int i = 0; i < info.containers_count; i++) {
    gui_container_info_t cinfo = info.containers[i];
    components_count += gui_container_info_get_components_count(cinfo);
  }

  component_t components[components_count];
  component_t *component_ptr = components;

  for (int i = 0; i < info.containers_count; i++) {
    gui_container_info_t cinfo = info.containers[i];

    uint16_t components_count = gui_container_info_get_components_count(cinfo);
    container_t container = gui_container_info_create(cinfo, component_ptr, components_count);
    gui_window_add_container(&window, container);

    component_ptr += components_count;
  }

  void *loop_state = info.construct(&window, state);

  window_t *prev_active_window = gui->active_window;

  gui->active_window = &window;
  gui_window_start_loop(gui, loop_running, job, loop_state);
  gui->active_window = prev_active_window;

  return true;
}

void gui_window_start_loop(gui_t *gui, gui_loop_running_fn loop_running,
                           gui_loop_job_fn job, void *loop_state) {
  while (loop_running(loop_state)) {
    commands_check_input(gui->commands);
    gui_update(gui);
    gui_render(gui);
    rgb_led_update(gui->pheripherals->rgb_leds);

    job(loop_state);
  }
}
