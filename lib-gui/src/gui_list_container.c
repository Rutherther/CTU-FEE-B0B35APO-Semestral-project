#include "gui.h"
#include "renderer.h"

container_t gui_list_container_create(void *state, uint32_t items_count,
                                      uint16_t item_height,
                                      render_item render_it,
                                      render_item render_header) {
  list_container_t list = {
    .item_height = item_height,
    .items_count = items_count,
    .state = state,
    .scroll_x = 0,
    .scroll_y = 0,
    .render_header_fn = render_header,
    .render_item_fn = render_it,
  };

  container_t container = {
    .focusable = true,
    .focused = false,
    .height = item_height * items_count,
    .width = 0,
    .inner.list = list,
    .x = 0,
    .y = 0
  };

  return container;
}

void gui_list_scroll(container_t *container, int16_t x, int16_t y) {
  container->inner.list.scroll_x += x;
  container->inner.list.scroll_y += y;
}

bool gui_list_container_set_state(container_t *container, void *state,
                                  uint32_t items_count) {
  container->inner.list.state = state;
  container->inner.list.items_count = items_count;
  return true;
}

bool gui_list_container_set_item_height(container_t *container,
                                        uint16_t item_height) {
  container->inner.list.item_height = item_height;
  return true;
}

bool gui_list_container_set_render_function(container_t *container, render_item render_it,
                                            render_item render_header) {
  container->inner.list.render_item_fn = render_it;
  container->inner.list.render_header_fn = render_header;
  return true;
}

void gui_list_container_render(gui_t *gui, container_t *container) {
  renderer_translate(gui->renderer, container->x, container->y);
  renderer_set_draw_area(gui->renderer, gui->size.x, gui->size.y);

  list_container_t list = container->inner.list;
  if (list.scroll_x < 0) {
    list.scroll_x = 0;
  }

  if (list.scroll_y < 0) {
    list.scroll_y = 0;
  }
  container->inner.list = list;

  uint16_t item_height = list.item_height;

  int32_t first_index = list.scroll_y / item_height;
  if (first_index < 0) {
    first_index = 0;
  }
  uint32_t items_count = gui->size.y / item_height;
  uint32_t end_index = first_index + items_count;

  int32_t beg_x = -list.scroll_x;
  int32_t beg_y = -list.scroll_y + first_index * item_height;

  for (int i = first_index; i < end_index; i++) {
    int32_t y = beg_y + i * item_height;

    list.render_item_fn(list.state, i, gui->renderer, beg_x, y);
  }
}

void gui_list_container_update(gui_t *gui, container_t *container) {
  // do nothing :)
}
