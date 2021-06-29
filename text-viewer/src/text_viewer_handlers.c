#include "text_viewer_handlers.h"
#include "direction.h"
#include "gui_component_text_view.h"
#include "keyboard_const.h"
#include "rotation_const.h"

static void command_handler_move(void *state, direction_t direction,
                                 int amount) {
  component_t *text_view = (component_t *)state;
  if (text_view->focused) {
    int32_t x = 0;
    int32_t y = 0;
    direction_move_xy(direction, &x, &y, amount);
    gui_text_view_scroll(text_view, x, y);
  }
}

static void command_handler_move_down(void *state, int amount) {
  command_handler_move(state, DOWN, amount);
}

static void command_handler_move_up(void *state, int amount) {
  command_handler_move(state, UP, amount);
}

static void command_handler_move_left(void *state, int amount) {
  command_handler_move(state, LEFT, amount);
}
static void command_handler_move_right(void *state, int amount) {
  command_handler_move(state, RIGHT, amount);
}

static void command_handler_reset(void *state, int amount) {
  gui_text_view_reset_scroll((component_t *)state);
}

static void command_handler_full_scroll(void *state, int amount) {
  gui_text_view_full_scroll((component_t *)state);
}

static void command_handler_zoom_in(void *state, int amount) {
  component_t *component = (component_t *)state;
  multiline_text_t* text = (multiline_text_t*) (component)->state;
  uint16_t old_size = text->font->size;

  amount = amount > 1 ? 1 : -1;
  text->font->size += amount;
  if (text->font->size == 0) {
    text->font->size = 1;
  }

  component->y += amount * (component->y / (old_size + text->font->line_spacing));
}

static void command_handler_zoom_out(void *state, int amount) {
  command_handler_zoom_in(state, -amount);
}

static void command_handler_zoom_reset(void *state, int amount) {
  component_t *component = (component_t *)state;
  multiline_text_t *text = (multiline_text_t *)(component)->state;
  uint16_t old_size = text->font->size;
  text->font->size = text->font->font.height;


  amount = text->font->size - old_size;
  component->y +=
      amount * (component->y / (old_size + text->font->line_spacing));
}

void gui_text_view_register_commands(gui_t *gui, component_t *text_view) {
  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_LEFT,
                    command_handler_move_left, text_view);
  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_RIGHT,
                    command_handler_move_right, text_view);
  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_DOWN,
                    command_handler_move_down, text_view);
  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_UP,
                    command_handler_move_up, text_view);
  commands_register(gui->commands, IN_KEYBOARD, 'r', command_handler_reset,
                    text_view);
  commands_register(gui->commands, IN_KEYBOARD, 't',
                    command_handler_full_scroll, text_view);
  commands_register(gui->commands, IN_KEYBOARD, 'f',
                    command_handler_zoom_reset, text_view);

  commands_register(gui->commands, IN_ENCODER_ROTATE,
                    ROTATION_ENCODER_HORIZONTAL, command_handler_move_right,
                    text_view);

  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_ZOOM_IN,
                    command_handler_zoom_in, text_view);
  commands_register(gui->commands, IN_KEYBOARD, KEYBOARD_ZOOM_OUT,
                    command_handler_zoom_out, text_view);

  commands_register(gui->commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_ZOOM,
                    command_handler_zoom_in, text_view);

  commands_register(gui->commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_VERTICAL,
                    command_handler_move_down, text_view);

  commands_register(gui->commands, IN_ENCODER_CLICK, ROTATION_ENCODER_VERTICAL,
                    command_handler_reset, text_view);
  commands_register(gui->commands, IN_ENCODER_CLICK, ROTATION_ENCODER_ZOOM,
                    command_handler_full_scroll, text_view);
}
