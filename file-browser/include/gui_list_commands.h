#ifndef __GUI_LIST_COMMANDS_H__
#define __GUI_LIST_COMMANDS_H__

#include <stdint.h>
#include "gui.h"

typedef void (*gui_list_item_clicked_fn)(container_t *list_container, void *state, uint32_t selected_index);

typedef struct {
  gui_list_item_clicked_fn clicked;
  void *state;
  container_t *container;

  gui_t *gui;
  window_t *window;
} gui_list_command_state_t;

void gui_list_commands_register(commands_t *commands, gui_list_command_state_t *state);

#endif // __GUI_LIST_COMMANDS_H__
