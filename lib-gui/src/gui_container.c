#include "gui.h"
#include "renderer.h"

void gui_container_render(gui_t *gui, container_t *container) {
  renderer_translate(gui->renderer, container->x, container->y);
  renderer_set_draw_area(gui->renderer, container->width, container->height);

  switch (container->type) {
  case CONT_TABLE:
    gui_list_container_render(gui, container);
    break;
  case CONT_GROUP:
    gui_group_container_render(gui, container);
    break;
  case CONT_ONE:
    gui_one_container_render(gui, container);
    break;
  }

  renderer_clear_translate(gui->renderer);
}

void gui_container_update(gui_t *gui, container_t *container) {
  switch (container->type) {
  case CONT_TABLE:
    gui_list_container_update(gui, container);
    break;
  case CONT_GROUP:
    gui_group_container_update(gui, container);
    break;
  case CONT_ONE:
    gui_one_container_update(gui, container);
    break;
  }
}
