#include "text_viewer.h"
#include "direction.h"
#include "display_utils.h"
#include "text_viewer_handlers.h"
#include "gui.h"
#include "gui_component_line.h"
#include "gui_component_text.h"
#include "gui_component_text_view.h"
#include "input.h"
#include "keyboard_const.h"
#include "logger.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include "renderer.h"
#include "rotation_const.h"
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *file_error_strings[] = {"Success",          "No such file or directory",
                              "No permissions",   "Cannot open file",
                              "Cannot read file", "Unknown"};

void file_error_log(logger_t *logger, file_error_t error) {
  logger_error(logger, __FILE__, __FUNCTION__, __LINE__, "There was an error loading file: %s", file_error_strings[error]);
}

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

component_t gui_text_view_create(gui_t *gui, multiline_text_t *text, int16_t x,
                                 int16_t y) {
  component_t text_view = gui_component_create(x, y, 1, 1, gui_text_view_render,
                                               gui_text_view_update);
  text_view.state = text;
  text_view.focusable = true;

  return text_view;
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
