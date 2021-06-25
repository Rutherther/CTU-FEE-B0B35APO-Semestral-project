#include "mzapo_rgb_led.h"
#include <stdlib.h>
#include <time.h>

mzapo_rgb_led_t rgb_led_create(unsigned char *mem_base) {
  mzapo_rgb_led_t rgb_led = {
    .mem_base = (volatile rgb_led_pixel_t*)mem_base,
  };

  for (int i = 0; i < RGB_LEDS_COUNT; i++) {
    rgb_led.states[i].timeout_ms = 0;
  }

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

void rgb_led_set_timeout(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id,
                         uint8_t r, uint8_t g, uint8_t b, uint32_t timeout_ms) {
  rgb_led_set(rgb_led, id, r, g, b);
  rgb_led->states[id].timeout_ms = timeout_ms;
  clock_gettime(CLOCK_MONOTONIC, &rgb_led->states[id].set_time);
}

void rgb_led_update(mzapo_rgb_led_t *rgb_led) {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);

  for (int i = 0; i < RGB_LEDS_COUNT; i++) {
    if (rgb_led->states[i].timeout_ms != 0) {
      struct timespec set_time = rgb_led->states[i].set_time;
      uint32_t diff = (((now.tv_sec - set_time.tv_sec) * 1000) +
                       (now.tv_nsec - set_time.tv_nsec)) /
                      1000000;

      if (diff >= rgb_led->states[i].timeout_ms) {
        rgb_led->states[i].timeout_ms = 0;
        rgb_led_clear(rgb_led, i);
      }
    }
  }
}
