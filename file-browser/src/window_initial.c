#include "window_initial.h"
#include "display_utils.h"
#include "font.h"
#include "gui.h"

#include "gui_window_info.h"
#include "gui_container_info.h"
#include "gui_list_commands.h"
#include "input.h"
#include "logger.h"
#include "renderer.h"
#include "window_browser.h"

#include <stdbool.h>

#define INITIAL_WINDOW_LOCAL_INDEX 0
#define INITIAL_WINDOW_MOUNT_INDEX 1
#define INITIAL_WINDOW_OPTIONS_INDEX 2
#define INITIAL_WINDOW_EXIT_INDEX 3

typedef struct {
  bool running;
  gui_t *gui;

  container_t *list_container;
  window_t *initial_window;

  font_t font;

  gui_list_command_state_t click_state;
} initial_window_state_t;

static bool initial_window_list_render_item(void *state, uint32_t index,
                                renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y, display_pixel_t color);

static void initial_window_item_clicked(container_t *container, void *state, uint32_t selected_index);

static void *initial_window_construct(window_t *window, void *state);
static bool initial_window_running(void *state);
static void initial_window_job(void *state);

gui_container_info_t initial_window_container_info = {
    .type = CONT_TABLE,
    .payload.list = {.item_height = 16,
                     .render_item_fn = initial_window_list_render_item,
                     .render_header_fn = NULL}};

window_info_t initial_window_info = {
  .containers_count = 1,
  .containers = &initial_window_container_info,
  .construct = initial_window_construct
};

bool window_initial_open(gui_t *gui, font_t font) {
  logger_t *logger = gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Opening initial window");

  initial_window_state_t state = {
    .running = true,
    .gui = gui,
    .font = font,
  };

  uint16_t commands_state = commands_save_state(gui->commands);
  bool success = gui_window_init_and_loop(gui, &state, initial_window_info, initial_window_running, initial_window_job);
  commands_restore_state(gui->commands, commands_state);
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Initial window closed");

  return success;
}

static bool initial_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y, display_pixel_t color) {
  initial_window_state_t *istate = (initial_window_state_t *)state;
  char *data;
  switch (index) {
  case INITIAL_WINDOW_LOCAL_INDEX:
    data = "ROOT FILESYSTEM";
    break;
  case INITIAL_WINDOW_MOUNT_INDEX:
    data = "MOUNTABLE DEVICES";
    break;
  case INITIAL_WINDOW_OPTIONS_INDEX:
    data = "OPTIONS";
    break;
  case INITIAL_WINDOW_EXIT_INDEX:
    data = "EXIT";
    break;
  default:
    return false;
  }

  renderer_write_string(renderer, beg_x, beg_y, 0, &istate->font, data, color);

  return true;
}

static void command_handler_exit(void *state, int amount) {
  initial_window_state_t *istate = (initial_window_state_t *)state;
  if (istate->gui->active_window == istate->initial_window) {
    istate->running = false;
  }
}

static void *initial_window_construct(window_t *window, void *state) {
  initial_window_state_t *istate = (initial_window_state_t*)state;
  logger_t *logger = istate->gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Constructing initial window");
  istate->list_container = &window->containers[0];
  istate->initial_window = window;

  istate->click_state.container = istate->list_container;
  istate->click_state.state = state;
  istate->click_state.clicked = initial_window_item_clicked;
  istate->click_state.font = &istate->font;
  istate->click_state.gui = istate->gui;
  istate->click_state.window = window;

  // containers init
  gui_container_info_init(istate->list_container, istate, 4, 5, 5);
  istate->list_container->width = istate->gui->size.x - 20;
  istate->list_container->height = istate->gui->size.y - 20;

  // commands register
  gui_list_commands_register(istate->gui->commands, &istate->click_state);
  commands_register(istate->gui->commands, IN_KEYBOARD, 'e', command_handler_exit, state);

  return state;
}

static bool initial_window_running(void *state) {
  initial_window_state_t *initial_window_state = (initial_window_state_t*)state;
  return initial_window_state->running;
}

static void initial_window_job(void *state) {
  initial_window_state_t *initial_window_state =
      (initial_window_state_t *)state;

  initial_window_state->list_container->inner.list.scroll_x = 0;
  gui_list_container_set_item_height(initial_window_state->list_container,
                                     initial_window_state->font.size);
  // do nothing?
}

static void initial_window_item_clicked(container_t *container, void *state,
                                        uint32_t selected_index) {
  initial_window_state_t *istate = (initial_window_state_t *)state;
  logger_t *logger = istate->gui->logger;

  if (istate->gui->active_window != istate->initial_window) {
    return;
  }

  switch (selected_index) {
  case INITIAL_WINDOW_LOCAL_INDEX:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Clicked local root filesystem");
    window_browser_open_local(istate->gui, &istate->font);
    break;
  case INITIAL_WINDOW_MOUNT_INDEX:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Clicked mount");
    break;
  case INITIAL_WINDOW_OPTIONS_INDEX:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Clicked options");
    break;
  case INITIAL_WINDOW_EXIT_INDEX:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Clicked exit");
    istate->running = false;
    break;
  default:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Clicked unknown item in initial window");
    break;
  }
}
