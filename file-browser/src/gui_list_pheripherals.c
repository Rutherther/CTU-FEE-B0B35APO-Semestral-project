#include "gui_list_pheripherals.h"
#include "gui.h"
#include "mzapo_led_strip.h"
#include <stdint.h>

#define LIST_LEDSTRIP_AROUND 1

void gui_list_ledstrip_update(container_t *container,
                              mzapo_pheripherals_t *pheripherals) {
  uint32_t selected_index = gui_list_get_selected_index(container);
  uint32_t items_count = gui_list_get_items_count(container);

  double perc = (double)selected_index/items_count;

  uint32_t index = LED_STRIP_COUNT * perc;
  ledstrip_turn_on(pheripherals->ledstrip, index, LIST_LEDSTRIP_AROUND);
}
