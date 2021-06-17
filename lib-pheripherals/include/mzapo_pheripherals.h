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

  mzapo_ledstrip_t mzapo_create_ledstrip();
  mzapo_rgb_led_t mzapo_create_rgb_led();
  display_t mzapo_create_display();

  void *mzapo_get_knobs_address();

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // __MZAPO_PHERIPHERALS_H__
