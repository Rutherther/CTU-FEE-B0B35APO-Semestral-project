#ifndef __GUI_WINDOW_H__
#define __GUI_WINDOW_H__

#include <stdint.h>
#include "gui.h"
#include "gui_container_info.h"

typedef void *(*construct_window_structure_fn)(window_t *window, void *state);
typedef bool (*gui_loop_running_fn)(void *state);
typedef void (*gui_loop_job_fn)(void *state);

typedef struct {
  uint32_t containers_count;
  construct_window_structure_fn construct;

  gui_container_info_t *containers;
} window_info_t;

bool gui_window_init_and_loop(gui_t *gui, void *state, window_info_t info,
                              gui_loop_running_fn loop_running,
                              gui_loop_job_fn job);

void gui_window_start_loop(gui_t * gui, gui_loop_running_fn loop_running,
                           gui_loop_job_fn job, void *loop_state);

#endif // __GUI_WINDOW_H__
