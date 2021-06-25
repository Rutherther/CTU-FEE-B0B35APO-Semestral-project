#ifndef __MZAPO_RGB_LED_H__
#define __MZAPO_RGB_LED_H__

#include "display_utils.h"
#include <time.h>

typedef enum { LED_LEFT, LED_RIGHT, RGB_LEDS_COUNT } mzapo_rgb_leds_t;

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} __attribute__((__packed__)) rgb_led_pixel_t;

typedef struct {
  uint32_t timeout_ms;
  struct timespec set_time;
} rgb_led_state_t;

typedef struct {
  volatile rgb_led_pixel_t *mem_base;
  rgb_led_state_t states[RGB_LEDS_COUNT];
} mzapo_rgb_led_t;

/**
 * @brief Create rgb leds with state
 *
 * @param mem_base virtual memory address where rgb leds data begin
 * @return mzapo_rgb_led_t
 */
mzapo_rgb_led_t rgb_led_create(unsigned char *mem_base);

/**
 * @brief Set given rgb led to rgb, 000 for turn off
 *
 * @param rgb_led
 * @param id what led to change
 * @param r red (min 0, max 255)
 * @param g green (min 0, max 255)
 * @param b blue (min 0, max 255)
 */
void rgb_led_set(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id, uint8_t r,
                 uint8_t g, uint8_t b);

/**
 * @brief Set given rgb led to rgb, then turn it off after timeout
 *
 * @param rgb_led
 * @param id what led to change
 * @param r red (min 0, max 255)
 * @param g green (min 0, max 255)
 * @param b blue (min 0, max 255)
 * @param timeout_ms milliseconds till led is turned off
 */
void rgb_led_set_timeout(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id, uint8_t r,
                         uint8_t g, uint8_t b, uint32_t timeout_ms);
/**
 * @brief Check if timeout was met so led should be turned off
 *
 * @param rgb_led
 */
void rgb_led_update(mzapo_rgb_led_t *rgb_led);

/**
 * @brief Set given rgb led to full red
 *
 * @param rgb_led
 * @param id what led to change
 */
void rgb_led_set_red(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

/**
 * @brief Set given rgb led to full green
 *
 * @param rgb_led
 * @param id what led to change
 */
void rgb_led_set_green(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

/**
 * @brief Set given rgb led to full blue
 *
 * @param rgb_led
 * @param id what led to change
 */
void rgb_led_set_blue(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

/**
 * @brief Get given rgb led color
 *
 * @param rgb_led
 * @param id what led to get data from
 * @return rgb_led_pixel_t
 */
rgb_led_pixel_t rgb_led_get(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

/**
 * @brief Set given rgb led to 0 - turn off
 *
 * @param rgb_led
 * @param id what led to change
 */
void rgb_led_clear(mzapo_rgb_led_t *rgb_led, mzapo_rgb_leds_t id);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_RGB_LED_H__
