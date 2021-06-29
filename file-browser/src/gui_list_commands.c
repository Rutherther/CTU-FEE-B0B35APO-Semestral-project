#include "gui_list_commands.h"
#include "direction.h"
#include "gui.h"
#include "input.h"
#include "keyboard_const.h"
#include "rotation_const.h"

static void command_handler_gui_list_clicked(void *state, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t*)state;
  if (amount > 1) {
    click_state->clicked(click_state->container, click_state->state,
                         gui_list_get_selected_index(click_state->container));
  }
}

static void command_handler_zoom_in(void *state,
                                 int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window &&
      click_state->font != NULL && amount != 0) {
    click_state->font->size += amount > 0 ? 1 : -1;
    gui_list_container_set_item_height(click_state->container,
                                       click_state->font->size);
  }
}

static void command_handler_zoom_out(void *state,
                                    int amount) {
  command_handler_zoom_in(state, -amount);
}

static void command_handler_move(void *state, direction_t direction, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window) {
    int32_t x = 0, y = 0;
    direction_move_xy(direction, &x, &y, amount);
    gui_list_scroll(click_state->container, (int16_t)x, (int16_t)y);
  }
}

static void command_handler_move_left(void *state, int amount) {
  command_handler_move(state, LEFT, amount);
}

static void command_handler_move_right(void *state, int amount) {
  command_handler_move(state, RIGHT, amount);
}

static void command_handler_move_down(void *state, int amount) {
  command_handler_move(state, DOWN, amount);
}

static void command_handler_move_up(void *state, int amount) {
  command_handler_move(state, UP, amount);
}

static void command_handler_home(void *state, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window) {
    gui_list_set_selected_index(click_state->container, 0);
  }
}

static void command_handler_end(void *state, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window) {
    gui_list_set_selected_index(click_state->container, gui_list_get_items_count(click_state->container));
  }
}

static void command_handler_page_down(void *state, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window) {
    uint32_t selected_index = gui_list_get_selected_index(click_state->container);
    gui_list_set_selected_index(
        click_state->container,
        selected_index +
            gui_list_get_visible_items_count(click_state->container));
  }
}

static void command_handler_page_up(void *state, int amount) {
  gui_list_command_state_t *click_state = (gui_list_command_state_t *)state;
  if (click_state->window == click_state->gui->active_window) {
    uint32_t selected_index =
        gui_list_get_selected_index(click_state->container);
    uint32_t visible_items = gui_list_get_visible_items_count(click_state->container);

    if (selected_index < visible_items) {
      selected_index = 0;
    } else {
      selected_index -= visible_items;
    }

    gui_list_set_selected_index(
        click_state->container,
        selected_index);
  }
}

void gui_list_commands_register(commands_t *commands, gui_list_command_state_t *state) {
  commands_register(commands, IN_KEYBOARD, 13, command_handler_gui_list_clicked, state);
  commands_register(commands, IN_KEYBOARD, 'v', command_handler_gui_list_clicked,
                    state);

  commands_register(commands, IN_KEYBOARD, KEYBOARD_HOME, command_handler_home,
                    state);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_END, command_handler_end,
                    state);

  commands_register(commands, IN_KEYBOARD, KEYBOARD_PAGE_DOWN,
                    command_handler_page_down, state);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_PAGE_UP,
                    command_handler_page_up, state);

  commands_register(commands, IN_KEYBOARD, KEYBOARD_DOWN,
                    command_handler_move_down, state);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_UP, command_handler_move_up,
                    state);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_LEFT,
                    command_handler_move_left, state);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_RIGHT,
                    command_handler_move_right, state);

  commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_HORIZONTAL,
                    command_handler_move_right, state);
  commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_VERTICAL,
                    command_handler_move_down, state);

  if (state->font != NULL) {
    commands_register(commands, IN_KEYBOARD, KEYBOARD_ZOOM_IN,
                      command_handler_zoom_in, state);
    commands_register(commands, IN_KEYBOARD, KEYBOARD_ZOOM_OUT,
                      command_handler_zoom_out, state);
    commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_ZOOM,
                      command_handler_zoom_in, state);
  }
}
