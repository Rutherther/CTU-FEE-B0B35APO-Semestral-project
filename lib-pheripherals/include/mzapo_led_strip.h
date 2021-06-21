#ifndef __MZAPO_LED_STRIP_H__
#define __MZAPO_LED_STRIP_H__

#include <stdint.h>

#define LED_STRIP_COUNT 32

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  volatile uint32_t *mem_base;
  uint8_t load;

  uint8_t on_index;
  uint8_t on_around_index;

  uint32_t strip;
} mzapo_ledstrip_t;

/**
 * @brief Create ledstrip with state
 *
 * @param mem_base virtual memory address where ledstrip data are saved
 * @return mzapo_ledstrip_t
 */
mzapo_ledstrip_t ledstrip_create(volatile uint32_t *mem_base);

/**
 * @brief Turn off all leds on ledstrip
 *
 * @param ledstrip
 */
void ledstrip_clear(mzapo_ledstrip_t *ledstrip);

/**
 * @brief Turn on led on index and some around it
 *
 * @param ledstrip
 * @param index what led to turn on
 * @param around how many leds around to left and right to turn on (around * 2 +
 * 1 leds will be turned on)
 */
void ledstrip_turn_on(mzapo_ledstrip_t *ledstrip, uint8_t index,
                      uint8_t around);

/**
 * @brief Turn on leds from 0 to steps to indicate progress bar
 *
 * @param ledstrip
 * @param steps how many steps out of LED_STRIP_COUNT
 */
void ledstrip_progress_bar_step(mzapo_ledstrip_t *ledstrip, int8_t steps);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_LED_STRIP_H__
