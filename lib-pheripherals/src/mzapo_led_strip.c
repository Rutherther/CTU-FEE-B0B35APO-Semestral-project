#include "mzapo_led_strip.h"
#include <stdlib.h>

mzapo_ledstrip_t ledstrip_create(volatile uint32_t *mem_base) {
  mzapo_ledstrip_t ledstrip = {
      .mem_base = mem_base,
      .load = 0,
      .on_index = 0xFF,
      .on_around_index = 0
  };

  return ledstrip;
}

void ledstrip_write(mzapo_ledstrip_t *ledstrip) {
  if (ledstrip->mem_base != NULL) {
    *ledstrip->mem_base = ledstrip->strip;
  }
}

void ledstrip_clear(mzapo_ledstrip_t *ledstrip) {
  ledstrip->strip = 0;
  ledstrip->load = 0;
  ledstrip->on_around_index = 0;
  ledstrip->on_index = 0xFF;

  ledstrip_write(ledstrip);
}

void ledstrip_turn_on(mzapo_ledstrip_t *ledstrip, uint8_t index,
                      uint8_t around) {
  ledstrip->on_index = index;
  ledstrip->on_around_index = around;
  ledstrip->strip = 0;
  index -= around;

  for (int i = 0; i < around * 2 + 1; i++) {
    if (index + i >= LED_STRIP_COUNT || index + i < 0) {
      
    }

    ledstrip->strip |= 1 << (LED_STRIP_COUNT - (i + index));
  }

  ledstrip_write(ledstrip);
}

void ledstrip_progress_bar_step(mzapo_ledstrip_t *ledstrip, int8_t steps) {
  ledstrip->strip = 0;
  ledstrip->load = steps;
  ledstrip->on_index = 0xFF;
  ledstrip->on_around_index = 0;

  for (int i = 0; i < ledstrip->load; i++) {
    ledstrip->strip |= 1 << i;
  }

  ledstrip_write(ledstrip);
}
