#include "gui_container_info.h"
#include "gui.h"

uint16_t gui_container_info_get_components_count(gui_container_info_t info) {
  if (info.type == CONT_GROUP) {
    return info.payload.group.components_count;
  }

  return 0;
}

container_t gui_container_info_create(gui_container_info_t info, component_t *components, uint16_t components_count) {
  switch(info.type) {
  case CONT_TABLE:
    {
    container_t container = gui_list_container_create(
        NULL, 0, info.payload.list.item_height,
        info.payload.list.render_item_fn, info.payload.list.render_header_fn);

    return container;
    }
    break;
  case CONT_ONE:
    return gui_one_container_create(0, 0);
  case CONT_GROUP:
    return gui_group_container_create(0, 0, components, components_count);
  }
}

void gui_container_info_init(container_t *container, void *state,
                             uint32_t items_count, int16_t x, int16_t y) {
  container->x = x;
  container->y = y;

  switch(container->type) {
  case CONT_ONE:
    // do nothing
    break;
  case CONT_GROUP:
    // do nothing
    break;
  case CONT_TABLE:
    gui_list_container_set_state(container, state, items_count);
    break;
  }
}
