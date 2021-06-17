#ifndef __MZAPO_RGB_LED_H__
#define __MZAPO_RGB_LED_H__

#include "display_utils.h"

typedef enum {
  LED_LEFT,
  LED_RIGHT
} mzapo_rgb_leds_t;

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
  } __attribute__((__packed__)) rgb_led_pixel_t;

  typedef struct {
    volatile rgb_led_pixel_t *mem_base;
  } mzapo_rgb_led_t;

  mzapo_rgb_led_t rgb_led_create(unsigned char *mem_base);

  void rgb_led_set(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id, uint8_t r,
                   uint8_t g, uint8_t b);
  void rgb_led_set_red(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);
  void rgb_led_set_green(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);
  void rgb_led_set_blue(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

  rgb_led_pixel_t rgb_led_get(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);
  void rgb_led_clear(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_RGB_LED_H__
