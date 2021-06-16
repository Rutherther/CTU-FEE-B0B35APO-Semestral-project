#include "display_utils.h"
#include "mzapo_parlcd.h"
#include <stdlib.h>

#ifdef COMPUTER
#include "xwin_sdl.h"
#endif

const display_pixel_t BLACK_PIXEL = {.bits = 0};
const display_pixel_t WHITE_PIXEL = {.bits = 0xFFFF};

const raw_pixel_t DISPLAY_PIXEL_MAX = {.red = (uint16_t)DISPLAY_MAX_RED,
                                       .green = (uint16_t)DISPLAY_MAX_GREEN,
                                       .blue = (uint16_t)DISPLAY_MAX_BLUE};

#ifdef COMPUTER
static void display_pixel_to_rgb(display_pixel_t pixel, uint8_t *target) {
  *(target++) = ((float)pixel.fields.r / DISPLAY_MAX_RED) * 255;
  *(target++) = ((float)pixel.fields.g / DISPLAY_MAX_GREEN) * 255;
  *(target++) = ((float)pixel.fields.b / DISPLAY_MAX_BLUE) * 255;
}
#endif

display_pixel_t raw_pixel_onebit_convert_to_display(raw_pixel_onebit_t pixel,
                                                    raw_pixel_onebit_t max) {
  display_pixel_t new = {
      .fields = {
          .r = ((float)pixel.red / (float)max.red) * DISPLAY_MAX_RED,
          .g = ((float)pixel.green / (float)max.green) * DISPLAY_MAX_GREEN,
          .b = ((float)pixel.blue / (float)max.blue) * DISPLAY_MAX_BLUE,
      }};

  return new;
}

display_pixel_t raw_pixel_convert_to_display(raw_pixel_t pixel,
                                             raw_pixel_t max) {
  display_pixel_t new = {
      .fields = {
          .r = ((float)pixel.red / (float)max.red) * DISPLAY_MAX_RED,
          .g = ((float)pixel.green / (float)max.green) * DISPLAY_MAX_GREEN,
          .b = ((float)pixel.blue / (float)max.blue) * DISPLAY_MAX_BLUE,
      }
  };

  return new;
}

display_t display_init(display_data_t data) {
  display_t display = {
    .data = data
  };

  if (data.base_address != NULL) {
    parlcd_hx8357_init(data.base_address);
    display_clear(&display, true);
  }

  #ifdef COMPUTER
  xwin_init(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  #endif

  return display;
}
void display_deinit(display_t *display) {
#ifdef COMPUTER
  xwin_close();
#else
  display_clear(display, true);
#endif
}

void display_render(display_t *display) {
  #ifdef COMPUTER
  uint8_t data[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3];
  for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    display_pixel_to_rgb(display->pixels[i], data + i * 3);
  }

  xwin_redraw(DISPLAY_WIDTH, DISPLAY_HEIGHT, data);
#else
  if (display->data.base_address == NULL) {
    return;
  }

  parlcd_write_cmd(display->data.base_address, PARLCD_CMD_FRAMEBUFFER);
  int count = DISPLAY_HEIGHT * DISPLAY_WIDTH;
  for (int i = 0; i < count; i++) {
    parlcd_write_data(display->data.base_address, display->pixels[i].bits);
  }
  #endif
}

void display_clear(display_t *display, bool render) {

  int count = DISPLAY_HEIGHT * DISPLAY_WIDTH;
  for (int i = 0; i < count; i++) {
    display->pixels[i] = BLACK_PIXEL;
  }

  if (render) {
    display_render(display);
  }
}

display_pixel_t display_get_pixel(display_t *display, uint16_t x, uint16_t y) {
  if (y >= DISPLAY_HEIGHT || x >= DISPLAY_WIDTH) {
    return BLACK_PIXEL;
  }

  return display->pixels[y * DISPLAY_WIDTH + x];
}

void display_set_pixel(display_t *display, uint16_t x, uint16_t y,
                       display_pixel_t pixel) {
  if (y >= DISPLAY_HEIGHT || x >= DISPLAY_WIDTH) {
    return;
  }

  display->pixels[y * DISPLAY_WIDTH + x] = pixel;
}
