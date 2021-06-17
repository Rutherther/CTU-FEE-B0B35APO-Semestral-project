#include "mzapo_rgb_led.h"
#include <stdlib.h>

mzapo_rgb_led_t rgb_led_create(unsigned char *mem_base) {
  mzapo_rgb_led_t rgb_led = {
    .mem_base = (volatile rgb_led_pixel_t*)mem_base,
  };

  return rgb_led;
}

void rgb_led_set(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id, uint8_t r,
                 uint8_t g, uint8_t b) {
  if (rgb_led->mem_base != NULL) {
    rgb_led_pixel_t *pixel = (rgb_led_pixel_t*)((void *)rgb_led->mem_base + id * 4);
    (pixel)->red = r;
    (pixel)->green = g;
    (pixel)->blue = b;
  }
}

void rgb_led_set_red(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id) {
  rgb_led_set(rgb_led, id, 255, 0, 0);
}
void rgb_led_set_green(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id) {
  rgb_led_set(rgb_led, id, 0, 255, 0);
}
void rgb_led_set_blue(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id) {
  rgb_led_set(rgb_led, id, 0, 0, 255);
}

rgb_led_pixel_t rgb_led_get(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id) {
  return *(rgb_led_pixel_t*)(((void*)rgb_led->mem_base + id * 4));
}

void rgb_led_clear(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id) {
  rgb_led_set(rgb_led, id, 0, 0, 0);
}
