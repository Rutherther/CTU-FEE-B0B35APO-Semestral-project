#include "text_viewer.h"
#include "direction.h"
#include "display_utils.h"
#include "gui.h"
#include "gui_component_line.h"
#include "gui_component_text.h"
#include "gui_component_text_view.h"
#include "input.h"
#include "keyboard_const.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include "renderer.h"
#include "rotation_const.h"
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

text_viewer_t text_viewer_create(char *path, mzapo_pheripherals_t pheripherals,
                                 logger_t *logger, font_t font) {
  text_viewer_t text_viewer = {.pheripherals = pheripherals,
                               .multiline_text = NULL,
                               .path = path,
                               .logger = logger,
                               .font = font};

  return text_viewer;
}

void text_viewer_destroy(text_viewer_t *text_viewer) {
  if (text_viewer->multiline_text != NULL &&
      text_viewer->multiline_text->text != NULL) {
    free(text_viewer->multiline_text->text);
  }
}

file_error_t file_error_from_errno() {
  switch (errno) {
  case ENOENT:
    return FILER_NOT_FOUND;
  case EACCES:
    return FILER_NO_PERMISSIONS;
  default:
    return FILER_FILE_CANT_OPEN;
  }
}

file_error_t text_viewer_load_file(text_viewer_t *text_viewer) {
  FILE *file = fopen(text_viewer->path, "r");

  if (file == NULL) {
    return file_error_from_errno();
  }

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *data = malloc(sizeof(char) * (fsize + 2));
  data[fsize - 1] = '\0';
  data[fsize] = '\0';
  data[fsize + 1] = '\0';

  if (data == NULL) {
    return FILER_UNKNOWN;
  }

  long read = 0;
  const int perc = 5;
  const int iters = 100 / perc;
  for (int i = 0; i < iters; i++) {
    long to_read = fsize / iters;
    if (to_read == 0) {
      i = iters - 1;
    }

    if (i == iters - 1) {
      to_read = fsize - read - 1;
    }

    if (to_read == 0 || fsize == read) {
      break;
    }

    long result = fread(data + read, sizeof(char), to_read, file);
    read += result;

    if (result != to_read) {
      fclose(file);
      return FILER_CANNOT_READ;
    }

    ledstrip_progress_bar_step(text_viewer->pheripherals.ledstrip, i * perc);
  }

  fclose(file);

  multiline_text_t *text =
      gui_multiline_text_create(&text_viewer->font, WHITE_PIXEL, data);
  if (text == NULL) {
    return FILER_UNKNOWN;
  }

  text_viewer->multiline_text = text;

  ledstrip_clear(text_viewer->pheripherals.ledstrip);
  return FILER_SUCCESS;
}

static void text_viewer_init_gui(text_viewer_t *text_viewer,
                                 commands_t *commands, renderer_t *renderer) {
  gui_t gui = gui_create(text_viewer->logger, commands, renderer,
                         &text_viewer->pheripherals);

  text_viewer->gui = gui;
}

static void command_handle_exit(void *state, int amount) {
  text_viewer_t *text_viewer = (text_viewer_t *)state;
  text_viewer->running = false;
}

static void text_viewer_register_commands(text_viewer_t *text_viewer,
                                          commands_t *commands) {
  commands_register(commands, IN_KEYBOARD, 'e', command_handle_exit,
                    text_viewer);
  commands_register(commands, IN_ENCODER_CLICK, 0, command_handle_exit,
                    text_viewer);
}

static container_t *
text_viewer_gui_add_name_and_line(text_viewer_t *text_viewer, window_t *window,
                                  text_t *name_text,
                                  component_t *name_and_line_components) {
  container_t name_and_line =
      gui_group_container_create(0, 0, name_and_line_components, 2);

  component_t name = gui_text_create(name_text, 5, 3, text_viewer->gui.size.x, 0);
  component_t line = gui_line_create(&WHITE_PIXEL, 0, name.height + name.y,
                                     text_viewer->gui.size.x, 1);

  name_and_line.width = text_viewer->gui.size.x;
  name_and_line.height = name.height + line.height + 3;
  container_t *name_and_line_ptr =
      gui_window_add_container(window, name_and_line);

  gui_group_container_add_component(name_and_line_ptr, name);
  gui_group_container_add_component(name_and_line_ptr, line);

  return name_and_line_ptr;
}

static container_t *text_viewer_gui_add_view_container(
    text_viewer_t *text_viewer, container_t *name_and_line, window_t *window) {
  container_t text_view_container =
      gui_one_container_create(0, name_and_line->y + name_and_line->height + 2);

  text_view_container.width = text_viewer->gui.size.x;
  text_view_container.height = text_viewer->gui.size.y - name_and_line->height;

  text_view_container.focused = true;
  return gui_window_add_container(window, text_view_container);
}

static component_t *text_viewer_gui_add_text_view(text_viewer_t *text_viewer,
                                                  container_t *view_container) {
  component_t text_view = gui_text_view_create(
      &text_viewer->gui, text_viewer->multiline_text, 0, 0);

  return gui_one_container_set_component(view_container, text_view);
}

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

component_t gui_text_view_create(gui_t *gui, multiline_text_t *text, int16_t x,
                                 int16_t y) {
  component_t text_view = gui_component_create(x, y, 1, 1, gui_text_view_render,
                                               gui_text_view_update);
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

void text_viewer_start_loop(text_viewer_t *text_viewer) {
  command_t command_arr[20];
  commands_t commands =
      commands_create(command_arr, 20, *text_viewer->pheripherals.knobs);
  renderer_t renderer = renderer_create(text_viewer->pheripherals.display);

  text_viewer_init_gui(text_viewer, &commands, &renderer);

  // window
  container_t containers[2];
  window_t text_viewer_window = gui_window_create(containers, 2);

  // name and line
  component_t name_and_line_components[2];
  font_t text_viewer_font_copy = text_viewer->font;
  text_t name_text = {.font = &text_viewer_font_copy,
                      .line = basename(text_viewer->path),
                      .color = WHITE_PIXEL};
  container_t *name_and_line = text_viewer_gui_add_name_and_line(
      text_viewer, &text_viewer_window, &name_text, name_and_line_components);

  // text viewer
  container_t *text_view_container = text_viewer_gui_add_view_container(
      text_viewer, name_and_line, &text_viewer_window);
  component_t *text_view =
      text_viewer_gui_add_text_view(text_viewer, text_view_container);

  gui_set_active_window(&text_viewer->gui, &text_viewer_window);

  gui_text_view_register_commands(&text_viewer->gui, text_view);

  text_viewer_register_commands(text_viewer, &commands);

  // main loop
  text_viewer->running = true;
  uint32_t prev_lines_scrolled = 0;
  while (text_viewer->running) {
    commands_check_input(&commands);

    gui_update(&text_viewer->gui);
    gui_render(&text_viewer->gui);
    rgb_led_update(text_viewer->pheripherals.rgb_leds);

    uint32_t lines_scrolled = gui_text_view_get_lines_scrolled(text_view);
    int32_t ledstrip_index =
        ((double)lines_scrolled /
         (text_viewer->multiline_text->lines_count -
          (double)text_viewer->gui.size.y /
              (text_viewer->multiline_text->font->size +
               text_viewer->multiline_text->font->line_spacing))) *
        LED_STRIP_COUNT;
    if (ledstrip_index > LED_STRIP_COUNT - 1) {
      ledstrip_index = LED_STRIP_COUNT - 1;
    }

    ledstrip_turn_on(text_viewer->pheripherals.ledstrip, ledstrip_index, 1);

    if (lines_scrolled != prev_lines_scrolled) {
      rgb_led_set_timeout(text_viewer->pheripherals.rgb_leds, LED_LEFT, 0, 100,
                          100, 150);
      rgb_led_set_timeout(text_viewer->pheripherals.rgb_leds, LED_RIGHT, 0, 100,
                          100, 150);

      prev_lines_scrolled = lines_scrolled;
    }
  }

  ledstrip_clear(text_viewer->pheripherals.ledstrip);
  rgb_led_clear(text_viewer->pheripherals.rgb_leds, LED_LEFT);
  rgb_led_clear(text_viewer->pheripherals.rgb_leds, LED_RIGHT);

  renderer_clear(&renderer);
  renderer_render(&renderer);
}
