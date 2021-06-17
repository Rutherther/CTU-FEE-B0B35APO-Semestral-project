#ifndef __MZAPO_LED_STRIP_H__
#define __MZAPO_LED_STRIP_H__

#include <stdint.h>

#define LED_STRIP_COUNT 30

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

  mzapo_ledstrip_t ledstrip_create(volatile uint32_t *mem_base);

  void ledstrip_clear(mzapo_ledstrip_t *ledstrip);
  void ledstrip_turn_on(mzapo_ledstrip_t *ledstrip, uint8_t index, uint8_t around);
  void ledstrip_progress_bar_step(mzapo_ledstrip_t *ledstrip, int8_t steps);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_LED_STRIP_H__
