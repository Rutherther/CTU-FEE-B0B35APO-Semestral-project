#include "file_access.h"
#include "file_browser_utils.h"
#include "file_open.h"
#include "font.h"
#include "gui.h"
#include "gui_component_text.h"
#include "gui_component_line.h"
#include "gui_list_commands.h"
#include "gui_window_info.h"
#include "renderer.h"
#include <stdbool.h>

#define CONTEXTMENU_OPEN_INDEX 0
#define CONTEXTMENU_OPEN_TEXT_INDEX 1
#define CONTEXTMENU_BACK_INDEX 2

typedef struct {
  bool running;
  gui_t *gui;

  fileaccess_state_t state;
  file_t *file;

  window_t *contextmenu_window;

  container_t *list_container;
  component_t *line_component;

  text_t name_state;

  font_t *font;

  gui_list_command_state_t click_state;
} contextmenu_window_state_t;

static bool contextmenu_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y,
                                            display_pixel_t color);

static void contextmenu_window_item_clicked(container_t *container, void *state,
                                        uint32_t selected_index);

static void *contextmenu_window_construct(window_t *window, void *state);
static bool contextmenu_window_running(void *state);
static void contextmenu_window_job(void *state);

gui_container_info_t contextmenu_window_container_info[] = {
    {.type = CONT_TABLE,
     .payload.list = {.item_height = 16,
                      .render_item_fn = contextmenu_window_list_render_item,
                      .render_header_fn = NULL}},
    {.type = CONT_GROUP, .payload.group.components_count = 2}};

window_info_t contextmenu_window_info = {
    .containers_count = 2,
    .containers = contextmenu_window_container_info,
    .construct = contextmenu_window_construct};

bool window_contextmenu_open(gui_t *gui, font_t *font, fileaccess_state_t state,
                             file_t *file) {
  logger_t *logger = gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Opening initial window");

  contextmenu_window_state_t cstate = {
      .running = true,
      .gui = gui,
      .font = font,
      .file = file,
      .state = state,
  };

  uint16_t commands_state = commands_save_state(gui->commands);
  bool success =
      gui_window_init_and_loop(gui, &cstate, contextmenu_window_info,
                               contextmenu_window_running, contextmenu_window_job);
  commands_restore_state(gui->commands, commands_state);
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Initial window closed");

  return success;
}

static bool contextmenu_window_list_render_item(void *state, uint32_t index,
                                                renderer_t *renderer,
                                                int16_t beg_x, int16_t beg_y,
                                                display_pixel_t color) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t *)state;
  char *data;
  switch (index) {
  case CONTEXTMENU_OPEN_INDEX:
    data = "OPEN";
    break;
  case CONTEXTMENU_OPEN_TEXT_INDEX:
    data = "SHOW TEXT";
    break;
  case CONTEXTMENU_BACK_INDEX:
    data = "BACK";
    break;
  default:
    return false;
  }

  renderer_write_string(renderer, beg_x, beg_y, 0, cstate->font, data, color);

  return true;
}

static void contextmenu_window_item_clicked(container_t *container, void *state,
                                            uint32_t selected_index) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t *)state;
  logger_t *logger = cstate->gui->logger;
  switch (selected_index) {
  case CONTEXTMENU_OPEN_INDEX:
    {
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Opening file");
    opened_file_state_t opened =
        file_open(cstate->file, browser_exec_options, cstate->state);
    file_browser_handle_opened_file(opened, cstate->gui, cstate->font);
    }
    break;
    case CONTEXTMENU_OPEN_TEXT_INDEX: {
      logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Opening text");
      opened_file_state_t opened =
          file_open_text(cstate->file, browser_exec_options, cstate->state);
      file_browser_handle_opened_file(opened, cstate->gui, cstate->font);
    }
    break;
  case CONTEXTMENU_BACK_INDEX:
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Going back");
    cstate->running = false;
    break;
  default:
    break;
  }
}

static void command_handler_exit(void *state, int amount) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t *)state;
  cstate->running = false;
}

static void *contextmenu_window_construct(window_t *window, void *state) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t *)state;
  logger_t *logger = cstate->gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Constructing contextmenu window");
  cstate->list_container = &window->containers[0];
  cstate->contextmenu_window = window;

  cstate->click_state.container = cstate->list_container;
  cstate->click_state.state = state;
  cstate->click_state.clicked = contextmenu_window_item_clicked;
  cstate->click_state.font = cstate->font;
  cstate->click_state.gui = cstate->gui;
  cstate->click_state.window = window;

  cstate->name_state.font = cstate->font;
  cstate->name_state.line = cstate->file->name;
  cstate->name_state.color = WHITE_PIXEL;

  // containers init
  // group components init
  component_t path_text = gui_text_create(&cstate->name_state, 3, 3, cstate->gui->size.x, 0);
  component_t line_component = gui_line_create(&WHITE_PIXEL, 0, path_text.height + path_text.y + 3, 1000, 1);

  gui_group_container_add_component(&window->containers[1], path_text);
  cstate->line_component =
      gui_group_container_add_component(&window->containers[1], line_component);

  // list init
  gui_container_info_init(cstate->list_container, cstate,
                          3, 5,
                          cstate->font->size / 2 + 3);
  cstate->list_container->width = cstate->gui->size.x - 20;
  cstate->list_container->height = cstate->gui->size.y - cstate->list_container->y - 20;

  // commands register
  gui_list_commands_register(cstate->gui->commands, &cstate->click_state);
  commands_register(cstate->gui->commands, IN_KEYBOARD, 'e',
                    command_handler_exit, state);

  return state;
}

static bool contextmenu_window_running(void *state) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t*)state;
  return cstate->running;
}

static void contextmenu_window_job(void *state) {
  contextmenu_window_state_t *cstate = (contextmenu_window_state_t *)state;
  cstate->line_component->y = cstate->font->size + 5;
  cstate->list_container->y = cstate->line_component->y / 2;
  gui_list_container_set_item_height(cstate->list_container,
                                     cstate->font->size);
}
