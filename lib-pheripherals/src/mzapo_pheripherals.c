#include "mzapo_pheripherals.h"
#include "display_utils.h"
#include "mzapo_led_strip.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "mzapo_rgb_led.h"
#include <stdlib.h>

mzapo_ledstrip_t mzapo_create_ledstrip() {
  void *pheripherals_base =
      map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  if (pheripherals_base != NULL) {
    pheripherals_base += SPILED_REG_LED_LINE_o;
  }

  return ledstrip_create(pheripherals_base);
}

mzapo_rgb_led_t mzapo_create_rgb_led() {
  void *pheripherals_base =
      map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  if (pheripherals_base != NULL) {
    pheripherals_base += SPILED_REG_LED_RGB1_o;
  }

  return rgb_led_create(pheripherals_base);
}

display_t mzapo_create_display() {
  display_data_t display_data = {
      .base_address =
          map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0),
  };

  return display_init(display_data);
}

void *mzapo_get_knobs_address() {
  void *pheripherals_base =
      map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

  if (pheripherals_base != NULL) {
    pheripherals_base += SPILED_REG_KNOBS_8BIT_o;
  }

  return pheripherals_base;
}

mzapo_pheripherals_t mzapo_pheripherals_create(mzapo_ledstrip_t *ledstrip,
                               mzapo_rgb_led_t *rgb_led, display_t *display,
                               void **mzapo_knobs_address) {
  mzapo_pheripherals_t pheripherals = {
    .display = display,
    .rgb_leds = rgb_led,
    .ledstrip = ledstrip,
    .knobs = mzapo_knobs_address
  };

  return pheripherals;
}

bool mzapo_check_pheripherals(mzapo_ledstrip_t *ledstrip,
                               mzapo_rgb_led_t *rgb_led, display_t *display,
                               void **mzapo_knobs_address) {
  #ifdef COMPUTER
  return true;
  #endif

  if (ledstrip != NULL && ledstrip->mem_base == NULL) {
    return false;
  }

  if (rgb_led != NULL && rgb_led->mem_base == NULL) {
    return false;
  }

  if (display != NULL && display->data.base_address == NULL) {
    return false;
  }

  if (mzapo_knobs_address != NULL && *mzapo_knobs_address == NULL) {
    return false;
  }

  return true;
}
