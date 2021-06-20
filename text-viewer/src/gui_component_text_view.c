#include "gui_component_text_view.h"
#include "display_utils.h"
#include "input.h"
#include "renderer.h"
#include <string.h>
#include <stdlib.h>
#include "direction.h"
#include "keyboard_const.h"
#include "rotation_const.h"

multiline_text_t *gui_multiline_text_create(font_t *font, display_pixel_t color, char *text) {
  size_t len = strlen(text) + 1;
  size_t lines = 1;

  for (int i = 0; i < len; i++) {
    if (text[i] == '\n') {
      lines++;
    }
  }

  void* new = realloc(text, len + sizeof(multiline_text_t) + sizeof(line_t) * lines);
  if (new == NULL) {
    free(text);
    return NULL;
  }

  text = new;

  multiline_text_t *multiline_text = (multiline_text_t*)(new + len);
  line_t *line_ptr = (line_t*)(new + len + sizeof(multiline_text_t));

  multiline_text->font = font;
  multiline_text->color = color;
  multiline_text->lines = line_ptr;
  multiline_text->lines_count = lines;
  multiline_text->text = text;

  bool new_line = false;
  uint32_t line_index = 0;
  uint32_t line_length = 0;
  char *line_str_ptr = text;

  for (int i = 0; i < len; i++) {
    if (new_line) {
      line_ptr[line_index].line = line_str_ptr;
      line_ptr[line_index++].length = line_length;
      line_str_ptr += line_length + 1;
      line_length = 0;
      new_line = false;
    }

    if (text[i] == '\n') {
      text[i] = '\0';
      new_line = true;
    } else {
      line_length++;
    }
  }

  if (line_index < lines) {
    line_ptr[line_index].line = line_str_ptr;
    line_ptr[line_index].length = line_length;
  }

  return multiline_text;
}

static void command_handler_move(void *state, direction_t direction, int amount) {
  component_t *text_view = (component_t*)state;
  if (text_view->focused) {
    direction_move_xy(direction, (int32_t*)&text_view->x, (int32_t*)&text_view->y, -amount);
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
  gui_text_view_reset_scroll((component_t*)state);
}

component_t gui_text_view_create(gui_t *gui, multiline_text_t *text, int16_t x, int16_t y) {
  component_t text_view =
      gui_component_create(x, y, 1, 1, gui_text_view_render, gui_text_view_update);
  text_view.state = text;
  text_view.focusable = true;

  return text_view;
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
  commands_register(gui->commands, IN_KEYBOARD, 'r',
                    command_handler_reset, text_view);

  commands_register(gui->commands, IN_ENCODER_ROTATE,
                    ROTATION_ENCODER_HORIZONTAL, command_handler_move_right,
                    text_view);

  commands_register(gui->commands, IN_ENCODER_ROTATE,
                    ROTATION_ENCODER_VERTICAL, command_handler_move_down,
                    text_view);

  commands_register(gui->commands, IN_ENCODER_CLICK,
                    ROTATION_ENCODER_ZOOM, command_handler_reset,
                    text_view);
}

void gui_text_view_scroll(component_t *text_view, int16_t x, int16_t y) {
  text_view->x -= x;
  text_view->y -= y;
}

void gui_text_view_reset_scroll(component_t *text_view) {
  text_view->x = 0;
  text_view->y = 0;
}

void gui_text_view_render(container_t *container, component_t *component,
                          gui_t *gui) {
  multiline_text_t *text = (multiline_text_t*)component->state;
  font_t *font = text->font;
  int16_t x_offset = component->x, y_offset = component->y;

  if (x_offset > 0) {
    x_offset = 0;
  }

  if (y_offset > 0) {
    y_offset = 0;
  } else if (y_offset < -(font->size + font->line_spacing) * (text->lines_count) + gui->size.y - container->y) {
    y_offset = -(font->size + font->line_spacing) * (text->lines_count) + gui->size.y - container->y;
  }

  component->x = x_offset;
  component->y = y_offset;

  int16_t first_line_index = -y_offset / (font->size + font->line_spacing);
  int16_t lines_count = gui->size.y / font->size;
  if (first_line_index < 0) {
    first_line_index = 0;
  }
  int16_t last_line_index = lines_count + first_line_index;

  y_offset += first_line_index * (font->size + font->line_spacing);

  for (int i = first_line_index; i < last_line_index && i < text->lines_count;
       i++) {
    line_t *line = &text->lines[i];
    size2d_t size = renderer_write_string(gui->renderer, x_offset, y_offset,
                                          line->length, font, line->line, text->color);

    y_offset += size.y;
  }
}

void gui_text_view_update(container_t *container, component_t *component,
                          gui_t *gui) {
  // do nothing
}
