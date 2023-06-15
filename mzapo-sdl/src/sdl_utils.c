#include "sdl_utils.h"
#include "image_load.h"
#include "sdl.h"
#include <limits.h>
#include <string.h>
#include <unistd.h>

#define IMG_PATH "./bin/mzapo.png"

buffer_pixel_t buffer[WINDOW_WIDTH * WINDOW_HEIGHT];
image_t image;

#define DISPLAY_MAX_RED 31u
#define DISPLAY_MAX_GREEN 63u
#define DISPLAY_MAX_BLUE 31u

void sdl_utils_init() {
  sdl_init(WINDOW_WIDTH, WINDOW_HEIGHT);
  printf(IMG_PATH);
  image = image_load_png(IMG_PATH);

  sdl_utils_clear_buffer();
}

void sdl_utils_deinit() {
  sdl_close();
}

void sdl_utils_render_buffer() {
  sdl_redraw(WINDOW_WIDTH, WINDOW_HEIGHT, (uint8_t*)buffer);
}

void sdl_utils_clear_buffer() {
  buffer_pixel_t black = {0, 0, 0};
  for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
    buffer[i] = black;
  }
}

void sdl_utils_draw_image() {
  for (uint16_t y = 0; y < WINDOW_HEIGHT && y < image.height; y++) {
    for (uint16_t x = 0; x < WINDOW_WIDTH && x < image.width; x++) {
      buffer[WINDOW_WIDTH * y + x] = image.data[image.width * y + x];
    }
  }
}

static void sdl_utils_rectangle(uint16_t bx, uint16_t by, uint16_t w, uint16_t h, buffer_pixel_t color) {
  for (uint16_t iy = 0; iy < h; iy++) {
    for (uint16_t ix = 0; ix < w; ix++) {
      uint16_t x = ix + bx;
      uint16_t y = iy + by;

      buffer[y * WINDOW_WIDTH + x] = color;
    }
  }
}

void sdl_utils_draw_ledstrip(uint32_t values) {
  float x = LEDSTRIP_START_X;
  float y = LEDSTRIP_START_Y;

  buffer_pixel_t color = {.r = 95, .g = 63, .b = 25};
  for (int i = 0; i < 32; i++) {
    bool on = (values >> (31 - i)) & 1;

    if (on) {
      sdl_utils_rectangle(x, y, LEDSTRIP_WIDTH, LEDSTRIP_HEIGHT, color);
    }

    x += LEDSTRIP_STEP_X;
    y += LEDSTRIP_STEP_Y;
  }
}

void sdl_utils_draw_display(uint16_t *data) {
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      uint16_t pixel = data[y * DISPLAY_WIDTH + x];

      uint8_t r = (pixel >> 11) & 0x1F;
      uint8_t g = (pixel >> 5) & 0x3F;
      uint8_t b = (pixel) & 0x1F;

      buffer_pixel_t convert = {
          .r = ((double)r / DISPLAY_MAX_RED) * 255,
          .g = ((double)g / DISPLAY_MAX_GREEN) * 255,
          .b = ((double)b / DISPLAY_MAX_BLUE) * 255,
      };

      buffer[(LCD_START_Y + y) * WINDOW_WIDTH + LCD_START_X + x] = convert;
    }
  }
}

void sdl_utils_draw_rgb_leds(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2,
                             uint8_t g2, uint8_t b2) {
  buffer_pixel_t left = {.r = r1, .g = g1, .b = b1};
  buffer_pixel_t right = {.r = r2, .g = g2, .b = b2};
  sdl_utils_rectangle(RGB_LEFT_LED_START_X, RGB_LEFT_LED_START_Y, RGB_LED_WIDTH,
                      RGB_LED_HEIGHT, left);
  sdl_utils_rectangle(RGB_RIGHT_LED_START_X, RGB_RIGHT_LED_START_Y,
                      RGB_LED_WIDTH, RGB_LED_HEIGHT, right);
}
