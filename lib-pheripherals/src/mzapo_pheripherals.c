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
