#include "dialog.h"
#include "display_utils.h"
#include "font.h"
#include "gui.h"
#include "gui_component_text.h"
#include "gui_container_info.h"
#include "gui_component_line.h"
#include "gui_window_info.h"
#include "input.h"
#include "logger.h"

#define DIALOG_WIDTH 350
#define DIALOG_HEIGHT 233
#define DIALOG_PADDING 10

typedef struct {
  bool running;
  gui_t *gui;

  window_t *dialog_window;
  component_t *title_component;
  component_t *dialog_component;

  text_t title_text;
  text_t dialog_text;

  font_t *font;

  const char *title;
  const char *text;
} dialog_state_t;

static void *dialog_construct(window_t *window, void *state);
static bool dialog_running(void *state);
static void dialog_job(void *state);

gui_container_info_t dialog_container_info[] = {
    {.type = CONT_GROUP, .payload.group.components_count = 3},
    {.type = CONT_ONE},
};

window_info_t dialog_info = {.containers_count = 2,
                             .containers = dialog_container_info,
                             .construct = dialog_construct};

bool dialog_info_show(gui_t *gui, font_t *font, const char *title, const char *text) {
  logger_info(gui->logger, __FILE__, __FUNCTION__, __LINE__, "Opening dialog");

  dialog_state_t state = {
      .running = true,
      .title = title,
      .text = text,
      .gui = gui,
      .font = font,
  };

  uint16_t commands_state = commands_save_state(gui->commands);
  bool success = gui_window_init_and_loop(gui, &state, dialog_info,
                                          dialog_running, dialog_job);
  commands_restore_state(gui->commands, commands_state);
  logger_info(gui->logger, __FILE__, __FUNCTION__, __LINE__, "Closing dialog");
  return success;
}

static void command_handler_close(void *state, int amount) {
  dialog_state_t *dstate = (dialog_state_t *)state;
  dstate->running = false;
}

static void *dialog_construct(window_t *window, void *state) {
  dialog_state_t *dstate = (dialog_state_t *)state;
  gui_t *gui = dstate->gui;
  font_t *font = dstate->font;
  dstate->dialog_window = window;

  dstate->title_text.font = font;
  dstate->title_text.color = WHITE_PIXEL;
  dstate->title_text.line = dstate->title;

  dstate->dialog_text.font = font;
  dstate->dialog_text.color = WHITE_PIXEL;
  dstate->dialog_text.line = dstate->text;

  container_t *one_container = &window->containers[1];
  container_t *group_container = &window->containers[0];

  int16_t begin_x = gui->size.x / 2 - DIALOG_WIDTH / 2;
  int16_t begin_y = gui->size.y / 2 - DIALOG_HEIGHT / 2;

  // construct line, rectangle and text
  gui_container_info_init(group_container, NULL, 0, begin_x,
                          begin_y);

  gui_group_container_add_component(
      group_container,
      gui_line_create(&WHITE_PIXEL, 0, 0, DIALOG_WIDTH, DIALOG_HEIGHT));

  gui_group_container_add_component(
      group_container,
      gui_line_create(&WHITE_PIXEL, 10, font->size + 3, DIALOG_WIDTH - 20, 1));

  uint16_t title_center_x =
      DIALOG_WIDTH / 2 - font_measure_text(font, dstate->title).x / 2;

  dstate->title_component = gui_group_container_add_component(
      group_container,
      gui_text_create(&dstate->title_text, title_center_x, 2, 0, 0));

  // construct message
  gui_container_info_init(one_container, NULL, 0, begin_x,
                          begin_y + font->size + 5);

  dstate->dialog_component = gui_one_container_set_component(
      one_container,
      gui_text_create(&dstate->dialog_text, DIALOG_PADDING, DIALOG_PADDING, DIALOG_WIDTH - 2*DIALOG_PADDING, DIALOG_HEIGHT));

  // register commands
  commands_register(gui->commands, IN_KEYBOARD, 0, command_handler_close, dstate);

  return state;
}

static bool dialog_running(void *state) {
  dialog_state_t *dstate = (dialog_state_t*)state;
  return dstate->running;
}

static void dialog_job(void *state) {
  dialog_state_t *dstate = (dialog_state_t *)state;
  //gui_t *gui = dstate->gui;
  font_t *font = dstate->font;

  uint16_t title_center_x =
      DIALOG_WIDTH / 2 - font_measure_text(font, dstate->title).x / 2;
  dstate->title_component->x = title_center_x;

  //int16_t begin_y = gui->size.y / 2 - DIALOG_HEIGHT / 2;
  //dstate->dialog_window->containers[0].y = begin_y + font->size + 5;
}
