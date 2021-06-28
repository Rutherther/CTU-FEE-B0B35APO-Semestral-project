#include "display_utils.h"
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
      .selected_index = 0,
      .render_header_fn = render_header,
      .render_item_fn = render_it,
      .regular_background = BLACK_PIXEL,
      .regular_foreground = WHITE_PIXEL,
      .selected_background = WHITE_PIXEL,
      .selected_foreground = BLACK_PIXEL,
      .item_padding = 3,
  };

  container_t container = {.focusable = true,
                           .focused = false,
                           .height = item_height * items_count,
                           .width = 0,
                           .inner.list = list,
                           .x = 0,
                           .y = 0};

  return container;
}

void gui_list_scroll(container_t *container, int16_t x, int16_t y) {
  container->inner.list.scroll_x += x;
  if (y < 0) {
    container->inner.list.selected_index--;
  } else if (y > 0) {
    container->inner.list.selected_index++;
  }
}

bool gui_list_container_set_state(container_t *container, void *state,
                                  uint32_t items_count) {
  container->inner.list.state = state;
  container->inner.list.items_count = items_count;
  container->inner.list.selected_index = 0;
  return true;
}

bool gui_list_container_set_item_height(container_t *container,
                                        uint16_t item_height) {
  container->inner.list.item_height = item_height;
  return true;
}

bool gui_list_container_set_render_function(container_t *container,
                                            render_item render_it,
                                            render_item render_header) {
  container->inner.list.render_item_fn = render_it;
  container->inner.list.render_header_fn = render_header;
  return true;
}

void gui_list_container_render(gui_t *gui, container_t *container) {
  renderer_translate(gui->renderer, container->x, container->y);
  renderer_set_draw_area(gui->renderer, container->width, container->height);

  list_container_t list = container->inner.list;
  if (list.scroll_x < 0) {
    list.scroll_x = 0;
  }

  if (list.scroll_y < 0) {
    list.scroll_y = 0;
  }
  container->inner.list = list;

  uint16_t item_height = list.item_height;
  uint16_t item_full_height = item_height + list.item_padding * 2;

  int32_t first_index = list.scroll_y / item_full_height;
  if (first_index < 0) {
    first_index = 0;
  }

  uint32_t items_count = gui->size.y / item_full_height + 1;
  uint32_t end_index = first_index + items_count;

  int32_t beg_x = -list.scroll_x;
  int32_t beg_y = -list.scroll_y + first_index * item_full_height;

  uint32_t selected_index = gui_list_get_selected_index(container);

  if (list.render_header_fn &&
      list.render_header_fn(list.state, 0, gui->renderer, beg_x + list.item_padding,
                            0 + list.item_padding, WHITE_PIXEL)) {
    // if header was rendered, translate initial position
    renderer_translate(gui->renderer, 0, item_full_height);
    renderer_set_draw_area(gui->renderer, container->width, container->height - item_full_height);
  }

  for (uint32_t i = first_index; i < end_index && i < list.items_count; i++) {
    int32_t y = beg_y + (i - first_index) * item_full_height;
    display_pixel_t fgcolor = list.regular_foreground;
    display_pixel_t bgcolor = list.regular_background;

    if (selected_index == i) {
      fgcolor = list.selected_foreground;
      bgcolor = list.selected_background;
    }

    renderer_render_rectangle(gui->renderer, beg_x, y, 1000, item_full_height,
                              bgcolor);
    list.render_item_fn(list.state, i, gui->renderer, beg_x + list.item_padding,
                        y + list.item_padding, fgcolor);
  }
}

uint32_t gui_list_get_selected_index(container_t *container) {
  return container->inner.list.selected_index;
}

void gui_list_container_update(gui_t *gui, container_t *container) {
  list_container_t list = container->inner.list;

  if (list.selected_index == UINT32_MAX) {
    list.selected_index = 0;
  } else if (list.selected_index > list.items_count - 1) {
    list.selected_index = list.items_count - 1;
  }

  uint16_t item_full_height = list.item_height + list.item_padding * 2;
  int32_t first_visible_index = list.scroll_y / item_full_height;
  if (first_visible_index < 0) {
    first_visible_index = 0;
  }

  uint32_t container_height = container->height;

  bool header = list.render_header_fn != NULL;
  if (header) {
    container_height -= item_full_height;
  }

  uint32_t items_count = (double)(container_height) / item_full_height - 1;
  uint32_t last_visible_index = first_visible_index + items_count;

  uint32_t selected_index = list.selected_index;

  if (selected_index < first_visible_index) {
    list.scroll_y = selected_index * item_full_height;
  } else if (selected_index > last_visible_index) {
    list.scroll_y = (selected_index - items_count) * item_full_height;
  }

  container->inner.list = list;
}
