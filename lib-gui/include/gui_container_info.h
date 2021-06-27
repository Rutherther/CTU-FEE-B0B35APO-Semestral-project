#ifndef __GUI_CONTAINER_H__
#define __GUI_CONTAINER_H__

#include "gui.h"

typedef struct {
  container_type_t type;
  union {
    struct {
      uint16_t components_count;
    } group;
    struct {
      render_item render_item_fn;
      render_item render_header_fn;
      uint16_t item_height;
    } list;
  } payload;
} gui_container_info_t;

uint16_t gui_container_info_get_components_count(gui_container_info_t info);
container_t gui_container_info_create(gui_container_info_t info, component_t *components, uint16_t components_count);
void gui_container_info_init(container_t *container, void *state, uint32_t items_count, int16_t x, int16_t y);

#endif // __GUI_CONTAINER_H__
