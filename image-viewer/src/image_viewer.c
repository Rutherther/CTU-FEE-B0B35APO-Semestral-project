#include "image_viewer.h"
#include "cursor.h"
#include "display_utils.h"
#include "image.h"
#include "input.h"
#include "logger.h"
#include <time.h>
#include <stdlib.h>

#define COMMANDS_NUM 30
#define CURSOR_SHOW_DURATION 3 // seconds

image_viewer_t image_viewer_create(char *filename, display_t *display, logger_t *logger) {
  image_viewer_t viewer = {
    .display = display,
    .image = image_create(filename),
    .cursor = cursor_create(),
    .region = image_region_create(0, 0, 0, 0),
    .logger = logger,
  };

  if (image_load(&viewer.image)) {
    viewer.region = image_region_create(0, 0, viewer.image.width, viewer.image.height);
  }

  return viewer;
}

void image_viewer_destroy(image_viewer_t *viewer) {
  image_destroy(&viewer->image);
}

void command_handler_move_left(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t*)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor to left by %d", amount);
  cursor_move(&viewer->cursor, &viewer->region, LEFT, amount);
  cursor_show(&viewer->cursor, viewer->display);
  display_render(viewer->display);
}

void command_handler_move_right(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor to right by %d", amount);
  cursor_move(&viewer->cursor, &viewer->region, RIGHT, amount);
  cursor_show(&viewer->cursor, viewer->display);
  display_render(viewer->display);
}

void command_handler_move_up(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor up by %d", amount);
  cursor_move(&viewer->cursor, &viewer->region, UP, amount);
  cursor_show(&viewer->cursor, viewer->display);
  display_render(viewer->display);
}

void command_handler_move_down(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor down by %d", amount);
  cursor_move(&viewer->cursor, &viewer->region, DOWN, amount);
  cursor_show(&viewer->cursor, viewer->display);
  display_render(viewer->display);
}

void command_handler_exit(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  viewer->running = false;
}

// TODO: implement zoom functions
void command_handler_zoom_in(void *data, int amount) {
  // hide cursor, zoom, show cursor
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zooming in by %d", amount);
}

void command_handler_zoom_out(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zooming out by %d", amount);
}

void command_handler_zoom_reset(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zoom reset by %d", amount);
}

void image_viewer_register_commands(image_viewer_t *viewer, commands_t *commands) {
  commands_register(commands, IN_KEYBOARD, 'h', &command_handler_move_left,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'j', &command_handler_move_down,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'k', &command_handler_move_up,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'l', &command_handler_move_right,
                    viewer);

  commands_register(commands, IN_KEYBOARD, 'e', &command_handler_exit,
                    viewer);

  commands_register(commands, IN_KEYBOARD, 'z', &command_handler_zoom_in,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'x', &command_handler_zoom_out,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'r', &command_handler_zoom_reset,
                    viewer);

  commands_register(commands, IN_ENCODER_ROTATE, 0, &command_handler_move_right,
                    viewer);
  commands_register(commands, IN_ENCODER_ROTATE, 1,
                    &command_handler_move_down, viewer);
  commands_register(commands, IN_ENCODER_ROTATE, 2, &command_handler_zoom_in,
                    viewer);

  commands_register(commands, IN_ENCODER_CLICK, 0, &command_handler_exit,
                    viewer);

  commands_register(commands, IN_ENCODER_CLICK, 2, &command_handler_zoom_reset,
                    viewer);
}

void image_viewer_start_loop(image_viewer_t *viewer, void *reg_knobs_base) {
  command_t command_array[COMMANDS_NUM];
  commands_t commands = commands_create(command_array, COMMANDS_NUM, reg_knobs_base);

  image_viewer_register_commands(viewer, &commands);

  viewer->running = true;
  time_t now = time(NULL);
  while (viewer->running) {
    // Main loop
    commands_check_input(&commands);
    now = time(NULL);

    if (viewer->cursor.shown && difftime(now, viewer->cursor.shown_at) >= CURSOR_SHOW_DURATION) {
      cursor_hide(&viewer->cursor, viewer->display);
      display_render(viewer->display);
    }
  }
}

void image_viewer_display_image(image_viewer_t *viewer) {
  image_write_to_display(&viewer->image, viewer->display, viewer->region);
}
