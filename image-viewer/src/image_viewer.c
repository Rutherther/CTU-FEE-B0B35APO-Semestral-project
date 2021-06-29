#include "image_viewer.h"
#include "cursor.h"
#include "direction.h"
#include "display_utils.h"
#include "image.h"
#include "input.h"
#include "logger.h"
#include "image_loader.h"
#include "coords.h"
#include "mzapo_led_strip.h"
#include "mzapo_rgb_led.h"
#include <time.h>
#include <stdlib.h>
#include "keyboard_const.h"
#include "rotation_const.h"

#define COMMANDS_NUM 30
#define CURSOR_SHOW_DURATION 3 // seconds

void loader_callback(void *state, double p) {
  mzapo_ledstrip_t *ledstrip = (mzapo_ledstrip_t*)state;
  ledstrip_progress_bar_step(ledstrip, p * LED_STRIP_COUNT);
}

image_viewer_t image_viewer_create(char *filename, display_t *display, logger_t *logger, mzapo_ledstrip_t ledstrip, mzapo_rgb_led_t rgb_leds) {

  image_viewer_t viewer = {
    .display = display,
    .image = image_create(filename),
    .cursor = cursor_create(),
    .logger = logger,
    .display_region = image_region_create(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT),
    .image_region = image_region_create(0, 0, 0, 0),
    .ledstrip = ledstrip,
    .rgb_leds = rgb_leds,
    .should_render = true,
  };

  viewer.error = image_loader_load(&viewer.image, loader_callback, &ledstrip);
  ledstrip_clear(&ledstrip);

  if (viewer.error == IMERR_SUCCESS) {
    viewer.image_region =
        image_region_create(0, 0, viewer.image.width, viewer.image.height);
    viewer.scale = image_get_initial_zoom(&viewer.image);
    cursor_center(&viewer.cursor, viewer.image_region);
  }

  return viewer;
}

void image_viewer_destroy(image_viewer_t *viewer) {
  image_destroy(&viewer->image);
}

void image_viewer_start_loop(image_viewer_t *viewer, void *reg_knobs_base) {
  command_t command_array[COMMANDS_NUM];
  commands_t commands = commands_create(command_array, COMMANDS_NUM, reg_knobs_base);

  image_viewer_register_commands(viewer, &commands);
  ledstrip_turn_on(
      &viewer->ledstrip,
      ((double)viewer->cursor.x / viewer->image.width) * LED_STRIP_COUNT, 1);

  viewer->running = true;
  time_t now = time(NULL);
  while (viewer->running) {
    // Main loop
    commands_check_input(&commands);
    now = time(NULL);

    if (viewer->cursor.shown && difftime(now, viewer->cursor.shown_at) >= CURSOR_SHOW_DURATION) {
      cursor_hide(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
      display_render(viewer->display);
    }
  }
}

