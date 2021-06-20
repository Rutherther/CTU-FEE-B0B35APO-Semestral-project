#include "text_viewer.h"
#include "display_utils.h"
#include "gui.h"
#include "gui_component_line.h"
#include "input.h"
#include "renderer.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui_component_text.h"
#include "gui_component_text_view.h"
#include <string.h>
#include <libgen.h>

text_viewer_t text_viewer_create(char *path, mzapo_pheripherals_t pheripherals,
                                 logger_t *logger, font_t font) {
  text_viewer_t text_viewer = {
      .pheripherals = pheripherals,
      .multiline_text = NULL,
      .path = path,
      .logger = logger,
      .font = font
  };

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

  char *data = malloc(sizeof(char) * (fsize + 1));
  data[fsize] = '\0';
  if (data == NULL) {
    return FILER_UNKNOWN;
  }

  long result = fread(data, sizeof(char), fsize, file);
  if (result != fsize) {
    fclose(file);
    return FILER_CANNOT_READ;
  }

  fclose(file);

  multiline_text_t *text = gui_multiline_text_create(&text_viewer->font, WHITE_PIXEL, data);
  if (text == NULL) {
    return FILER_UNKNOWN;
  }

  text_viewer->multiline_text = text;

  return FILER_SUCCESS;
}

static void text_viewer_init_gui(text_viewer_t *text_viewer,
                                 commands_t *commands, renderer_t *renderer) {
  gui_t gui = gui_create(text_viewer->logger, commands, renderer,
                         &text_viewer->pheripherals);
 
  text_viewer->gui = gui;
}

static void command_handle_exit(void *state, int amount) {
  text_viewer_t *text_viewer = (text_viewer_t*)state;
  text_viewer->running = false;
}

static void text_viewer_register_commands(text_viewer_t *text_viewer, commands_t *commands) {
  commands_register(commands, IN_KEYBOARD, 'e', command_handle_exit,
                    text_viewer);
  commands_register(commands, IN_ENCODER_CLICK, 0, command_handle_exit,
                    text_viewer);
}

static container_t *text_viewer_gui_add_name_and_line(text_viewer_t *text_viewer, window_t *window, text_t *name_text, component_t *name_and_line_components) {
  container_t name_and_line =
      gui_group_container_create(0, 0, name_and_line_components, 2);

  component_t name = gui_text_create(name_text, 5, 3, 0, 0);
  component_t line = gui_line_create(&WHITE_PIXEL, 0, name.height + name.y,
                                     text_viewer->gui.size.x, 1);

  gui_group_container_add_component(&name_and_line, name);
  gui_group_container_add_component(&name_and_line, line);

  name_and_line.width = text_viewer->gui.size.x;
  name_and_line.height = name.height + line.height + 3;
  return gui_window_add_container(window, name_and_line);
}

static container_t *text_viewer_gui_add_view_container(text_viewer_t *text_viewer, container_t *name_and_line, window_t *window) {
  container_t text_view_container =
      gui_one_container_create(0, name_and_line->y + name_and_line->height + 2);

  text_view_container.width = text_viewer->gui.size.x;
  text_view_container.height = text_viewer->gui.size.y - name_and_line->height;

  text_view_container.focused = true;
  return gui_window_add_container(window, text_view_container);
}

static component_t *text_viewer_gui_add_text_view(text_viewer_t *text_viewer, container_t *view_container) {
  component_t text_view = gui_text_view_create(
      &text_viewer->gui, text_viewer->multiline_text, 0, 0);

  return gui_one_container_set_component(view_container, text_view);
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
  text_t name_text = {.font = &text_viewer->font,
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

  gui_text_view_register_commands(&text_viewer->gui,
                                  text_view); // TODO: rethink this design

  text_viewer_register_commands(text_viewer, &commands);

  // main loop
  text_viewer->running = true;
  while (text_viewer->running) {
    commands_check_input(&commands);

    gui_update(&text_viewer->gui);
    gui_render(&text_viewer->gui);
  }

  renderer_clear(&renderer);
  renderer_render(&renderer);
}
