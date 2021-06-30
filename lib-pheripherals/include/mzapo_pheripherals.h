#ifndef __MZAPO_PHERIPHERALS_H__
#define __MZAPO_PHERIPHERALS_H__

#include "mzapo_led_strip.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_rgb_led.h"
#include "display_utils.h"

#ifdef __cplusplus
extern "C" {
#endif
  typedef struct {
    display_t *display;
    mzapo_rgb_led_t *rgb_leds;
    mzapo_ledstrip_t *ledstrip;
    void **knobs;
  } mzapo_pheripherals_t;

  mzapo_ledstrip_t mzapo_create_ledstrip();
  mzapo_rgb_led_t mzapo_create_rgb_led();
  display_t mzapo_create_display();

  void *mzapo_get_knobs_address();

  mzapo_pheripherals_t mzapo_pheripherals_create(mzapo_ledstrip_t *ledstrip, mzapo_rgb_led_t *rgb_led, display_t *display, void **mzapo_knobs_address);
  bool mzapo_check_pheripherals(mzapo_ledstrip_t *ledstrip,
                                mzapo_rgb_led_t *rgb_led, display_t *display,
                                void **mzapo_knobs_address);

  void mzapo_pheripherals_clear(mzapo_pheripherals_t *pheripherals);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_PHERIPHERALS_H__
