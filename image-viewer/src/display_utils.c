#include "display_utils.h"
#include "mzapo_parlcd.h"

const display_pixel_t BLACK_PIXEL = {.bits = 0};
const display_pixel_t WHITE_PIXEL = {.bits = 0xFFFF};

const raw_pixel_t DISPLAY_PIXEL_MAX = {.red = (uint16_t)DISPLAY_MAX_RED,
                                       .green = (uint16_t)DISPLAY_MAX_GREEN,
                                       .blue = (uint16_t)DISPLAY_MAX_BLUE};

display_pixel_t raw_pixel_onebit_convert_to_display(raw_pixel_onebit_t pixel,
                                                    raw_pixel_onebit_t max) {
  display_pixel_t new = {
      .fields = {
          .r = ((double)pixel.red / (double)max.red) * DISPLAY_MAX_RED,
          .g = ((double)pixel.green / (double)max.green) * DISPLAY_MAX_GREEN,
          .b = ((double)pixel.blue / (double)max.blue) * DISPLAY_MAX_BLUE,
      }};

  return new;
}

display_pixel_t raw_pixel_convert_to_display(raw_pixel_t pixel,
                                             raw_pixel_t max) {
  display_pixel_t new = {
      .fields = {
          .r = ((double)pixel.red / (double)max.red) * DISPLAY_MAX_RED,
          .g = ((double)pixel.green / (double)max.green) * DISPLAY_MAX_GREEN,
          .b = ((double)pixel.blue / (double)max.blue) * DISPLAY_MAX_BLUE,
      }
  };

  return new;
}

display_t display_init(display_data_t data) {
  display_t display = {
    .data = data
  };
  parlcd_hx8357_init(data.base_address);
  display_clear(&display);

  return display;
}
void display_deinit(display_t *display) {
  display_clear(display);
}

void display_render(display_t *display) {
  parlcd_write_cmd(display->data.base_address, PARLCD_CMD_FRAMEBUFFER);
  int count = DISPLAY_HEIGHT * DISPLAY_WIDTH;
  for (int i = 0; i < count; i++) {
    parlcd_write_data(display->data.base_address, display->pixels[i].bits);
  }
}

void display_clear(display_t *display) {
  int count = DISPLAY_HEIGHT * DISPLAY_WIDTH;
  for (int i = 0; i < count; i++) {
    display->pixels[i] = BLACK_PIXEL;
  }

  display_render(display);
}

display_pixel_t display_get_pixel(display_t *display, uint16_t x, uint16_t y) {
  return display->pixels[y * DISPLAY_WIDTH + x];
}

void display_set_pixel(display_t *display, uint16_t x, uint16_t y,
                       display_pixel_t pixel) {
  display->pixels[y * DISPLAY_WIDTH + x] = pixel;
}
