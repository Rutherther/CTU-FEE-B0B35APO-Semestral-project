#include "window_browser.h"
#include "display_utils.h"
#include "file_access.h"
#include "file_open.h"
#include "gui.h"
#include "gui_component_text.h"
#include "gui_container_info.h"
#include "gui_list_commands.h"
#include "gui_component_line.h"
#include "gui_window_info.h"
#include "input.h"
#include "logger.h"
#include "path.h"
#include "renderer.h"

typedef struct {
  bool running;
  gui_t *gui;

  container_t *list_container;
  window_t *browser_window;

  font_t *font;

  gui_list_command_state_t click_state;
  text_t text_state;

  directory_t *current_directory;
  fileaccess_state_t state;
} browser_window_state_t;

static bool browser_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y,
                                            display_pixel_t color);

static bool browser_window_list_render_header(void *state, uint32_t index,
                                              renderer_t *renderer,
                                              int16_t beg_x, int16_t beg_y,
                                              display_pixel_t color);

static void browser_window_item_clicked(container_t *container, void *state,
                                        uint32_t selected_index);

static void *browser_window_construct(window_t *window, void *state);
static bool browser_window_running(void *state);
static void browser_window_job(void *state);

gui_container_info_t window_browser_containers[] = {
    {.type = CONT_TABLE,
     .payload.list = {.render_item_fn = browser_window_list_render_item,
                      .render_header_fn = browser_window_list_render_header,
                      .item_height = 16}},
    {.type = CONT_GROUP, .payload.group.components_count = 2},
};

window_info_t window_browser_info = {
    .construct = browser_window_construct,
    .containers_count = 2,
    .containers = window_browser_containers,
};

bool window_browser_open_local(gui_t *gui, font_t *font) {
  fileaccess_state_t state = fileaccess_init(&local_file_access, NULL);
  return window_browser_open(gui, font, state);
}

bool window_browser_open(gui_t *gui, font_t *font, fileaccess_state_t state) {
  directory_or_error_t root = fileaccess_root_list(state);

  if (root.error) {
    fileaccess_log_error(gui->logger, root.error);
    // TODO: dialog
    return false;
  }

  browser_window_state_t bstate = {
      .state = state,
      .gui = gui,
      .font = font,
      .current_directory = root.payload.directory,
      .running = true,
  };

  uint16_t commands_state = commands_save_state(gui->commands);
  gui_window_init_and_loop(gui, &bstate, window_browser_info, browser_window_running,
                           browser_window_job);
  commands_restore_state(gui->commands, commands_state);

  return true;
}

static void command_handler_exit(void *state, int amount) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  if (bstate->gui->active_window == bstate->browser_window) {
    bstate->running = false;
  }
}

static void *browser_window_construct(window_t *window, void *state) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  logger_t *logger = bstate->gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Constructing browser window");
  bstate->list_container = &window->containers[0];
  bstate->browser_window = window;

  bstate->click_state.container = bstate->list_container;
  bstate->click_state.state = state;
  bstate->click_state.clicked = browser_window_item_clicked;
  bstate->click_state.font = bstate->font;
  bstate->click_state.gui = bstate->gui;
  bstate->click_state.window = window;

  bstate->text_state.font = bstate->font;
  bstate->text_state.line = bstate->current_directory->path;
  bstate->text_state.color = WHITE_PIXEL;

  // containers init
  // group components init
  component_t path_text = gui_text_create(&bstate->text_state, 3, 3, 0, 0);
  component_t line_component = gui_line_create(&WHITE_PIXEL, 0, path_text.height + path_text.y + 3, 1000, 1);
  
  gui_group_container_add_component(&window->containers[1], path_text);
  gui_group_container_add_component(&window->containers[1], line_component);

  // list init
  gui_container_info_init(bstate->list_container, bstate,
                          bstate->current_directory->files_count, 5,
                          5 + line_component.height + 5);
  bstate->list_container->width = bstate->gui->size.x - 20;
  bstate->list_container->height = bstate->gui->size.y - bstate->list_container->y - 20;

  // commands register
  gui_list_commands_register(bstate->gui->commands, &bstate->click_state);
  commands_register(bstate->gui->commands, IN_KEYBOARD, 'e',
                    command_handler_exit, state);

  return state;
}

static void browser_window_item_clicked(container_t *container, void *state,
                                        uint32_t selected_index) {

  browser_window_state_t *bstate = (browser_window_state_t *)state;
  logger_t *logger = bstate->gui->logger;

  file_t current_file = bstate->current_directory->files[selected_index];

  if (current_file.type == FT_FILE) {
    // open
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Opening file %s",
                current_file.name);
    file_operation_error_t error = file_open(&current_file, browser_exec_options, bstate->state);
    if (error != FILOPER_SUCCESS) {
      fileaccess_log_error(logger, error);
    } else {
      logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Successfully returned from executing file.");
    }
  } else if (current_file.type == FT_FOLDER || current_file.type == FT_OTHER) {
    char new_dir_path[path_join_memory_size(bstate->current_directory->path, current_file.name)];
    path_join(bstate->current_directory->path, current_file.name, new_dir_path);

    directory_or_error_t data = fileaccess_directory_list(bstate->state, new_dir_path);
    if (data.error) {
      // show error
      fileaccess_log_error(logger, data.payload.error);
    } else {
      fileaccess_directory_close(bstate->state, bstate->current_directory);
      bstate->current_directory = data.payload.directory;
      bstate->text_state.line = bstate->current_directory->path;

      gui_list_container_set_state(bstate->list_container, bstate, bstate->current_directory->files_count);

      logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Opening directory %s", bstate->current_directory->path);
    }
  }
}

static bool browser_window_list_render_item(void *state, uint32_t index,
                                            renderer_t *renderer, int16_t beg_x,
                                            int16_t beg_y,
                                            display_pixel_t color) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  logger_t *logger = bstate->gui->logger;
  if (index >= bstate->current_directory->files_count) {
    logger_error(logger, __FILE__, __FUNCTION__, __LINE__, "Tried to reach item out of index");
    return false;
  }
  file_t file = bstate->current_directory->files[index];
  renderer_write_string(renderer, beg_x, beg_y, 0, bstate->font, file.name,
                        color);
  return true;
}

static bool browser_window_list_render_header(void *state, uint32_t index,
                                              renderer_t *renderer,
                                              int16_t beg_x, int16_t beg_y,
                                              display_pixel_t color) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  renderer_write_string(renderer, beg_x, beg_y, 0, bstate->font, "This is header", color);
  return true;
}

static bool browser_window_running(void *state) {
  browser_window_state_t *bstate = (browser_window_state_t*)state;
  return bstate->running;
}

static void browser_window_job(void *state) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  if (!bstate->running) {
    // cleanup
    fileaccess_directory_close(bstate->state, bstate->current_directory);
  }
}
