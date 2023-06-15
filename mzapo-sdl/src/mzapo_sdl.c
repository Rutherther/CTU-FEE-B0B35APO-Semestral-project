#include "mzapo_sdl.h"
#include "mzapo_regs.h"
#include "sdl_utils.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

static pthread_t sdl_thread;
static bool running;

static bool display_draw = false;
static uint16_t display_x = 0, display_y = 0;

static uint16_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];

static void *mzapo_sdl_thread_init(void *state);

static void mzapo_display_cmd(uint16_t cmd);
static void mzapo_display_data(uint16_t data);

mzapo_sdl_display display = {.cmd = mzapo_display_cmd, .data = mzapo_display_data};
mzapo_sdl_knobs_keyboard_rgb sdl_knobs_keyboard_rgb = {
  .reg_knobs_direct = 0, .reg_kbdrd_direct = 0,
    .reg_knobs_8bit = 0, .ledstrip = 0 };

void mzapo_sdl_init() {
  running = true;

  sdl_knobs_keyboard_rgb.leds.left.r = 0;
  sdl_knobs_keyboard_rgb.leds.left.g = 0;
  sdl_knobs_keyboard_rgb.leds.left.b = 0;

  sdl_knobs_keyboard_rgb.leds.right.r = 0;
  sdl_knobs_keyboard_rgb.leds.right.g = 0;
  sdl_knobs_keyboard_rgb.leds.right.b = 0;

  for (int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++) {
    display_buffer[i] = 0;
  }

  pthread_create(&sdl_thread, NULL, mzapo_sdl_thread_init, NULL);
}

void mzapo_sdl_deinit() {
  running = false;
  pthread_join(sdl_thread, NULL);
}

void *mzapo_sdl_map_phys(uint64_t region_base, uint64_t region_size) {
  switch (region_base) {
  case PARLCD_REG_BASE_PHYS:
    return &display;
  case SPILED_REG_BASE_PHYS:
    return &sdl_knobs_keyboard_rgb;
  default:
    return NULL;
  }
}

static void *mzapo_sdl_thread_init(void *state) {
  sdl_utils_init();
  while (running) {
    sdl_utils_clear_buffer();

    sdl_utils_draw_image();

    sdl_utils_draw_rgb_leds(sdl_knobs_keyboard_rgb.leds.left.r,
                            sdl_knobs_keyboard_rgb.leds.left.g,
                            sdl_knobs_keyboard_rgb.leds.left.b,
                            sdl_knobs_keyboard_rgb.leds.right.r,
                            sdl_knobs_keyboard_rgb.leds.right.g,
                            sdl_knobs_keyboard_rgb.leds.right.b);

    sdl_utils_draw_ledstrip(sdl_knobs_keyboard_rgb.ledstrip);

    sdl_utils_draw_display(display_buffer);

    sdl_utils_render_buffer();
  }

  return NULL;
}

static void mzapo_display_cmd(uint16_t cmd) {
  switch(cmd) {
  case 0x2C:
    display_x = 0;
    display_y = 0;
    display_draw = true;
    break;
  }
}

static void mzapo_display_data(uint16_t data) {
  if (display_draw && display_y < DISPLAY_HEIGHT) {
    display_buffer[display_y * DISPLAY_WIDTH + display_x] = data;

    display_x ++;

    if (display_x >= DISPLAY_WIDTH) {
      display_x = 0;
      display_y ++;
    }
  }
}
