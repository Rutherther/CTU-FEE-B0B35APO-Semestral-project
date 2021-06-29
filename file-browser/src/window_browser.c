#include "window_browser.h"
#include "dialog.h"
#include "file_browser_utils.h"
#include "display_utils.h"
#include "window_contextmenu.h"
#include "gui_list_table.h"
#include "file_access.h"
#include "file_open.h"
#include "font.h"
#include "gui.h"
#include "gui_component_text.h"
#include "gui_container_info.h"
#include "gui_list_commands.h"
#include "gui_component_line.h"
#include "gui_list_pheripherals.h"
#include "gui_window_info.h"
#include "input.h"
#include "logger.h"
#include "mzapo_rgb_led.h"
#include "path.h"
#include "renderer.h"
#include "keyboard_const.h"
#include "window_browser_items.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

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
    fileaccess_log_error(gui->logger, root.payload.error);
    dialog_info_show(gui, font, "Could not open root directory",
                     fileaccess_get_error_text(root.payload.error));
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

static void command_handler_contextmenu(void *state, int amount) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  if (bstate->gui->active_window == bstate->browser_window) {
    uint32_t selected_index = gui_list_get_selected_index(bstate->list_container);
    file_t *file = &bstate->current_directory->files[selected_index];

    if (file->type != FT_FILE) {
      logger_warn(bstate->gui->logger, __FILE__, __FUNCTION__, __LINE__,
                  "Context menus are for files only");
      return;
    }

    logger_info(bstate->gui->logger, __FILE__, __FUNCTION__, __LINE__,
                "Context menu will open");

    window_contextmenu_open(bstate->gui, bstate->font, bstate->state, file);
  }
}

static void command_handler_jump_right(void *state, int amount) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  if (bstate->gui->active_window == bstate->browser_window) {
    uint32_t scroll_x = bstate->list_container->inner.list.scroll_x;
    uint32_t current_x = 0;

    int i;
    for (i = 0; i < COLUMNS_COUNT - 1; i++) {
      current_x += bstate->column_widths[i];
      if (scroll_x < current_x) {
        break;
      }
    }

    if (amount < 0) {
      i --;
    } else {
      i++;
    }

    if (i < 0) {
      i = 0;
    } else if (i > COLUMNS_COUNT - 1) {
      i = COLUMNS_COUNT - 1;
    }

    scroll_x = 0;
    for (int j = 0; j < i; j++) {
      scroll_x += bstate->column_widths[j];
    }

    bstate->list_container->inner.list.scroll_x = scroll_x;
  }
}

static void command_handler_jump_left(void *state, int amount) {
  command_handler_jump_right(state, -amount);
}

static void *browser_window_construct(window_t *window, void *state) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;
  logger_t *logger = bstate->gui->logger;
  logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
              "Constructing browser window");
  bstate->list_container = &window->containers[0];
  bstate->browser_window = window;

  bstate->table.columns_count = COLUMNS_COUNT;
  bstate->table.columns_names = column_names;
  bstate->table.columns_widths = bstate->column_widths;
  bstate->table.get_data = browser_get_column_data;

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
  component_t path_text = gui_text_create(&bstate->text_state, 3, 3, bstate->gui->size.x, 0);
  component_t line_component = gui_line_create(&WHITE_PIXEL, 0, path_text.height + path_text.y + 3, 1000, 1);

  gui_group_container_add_component(&window->containers[1], path_text);
  bstate->line_component =
      gui_group_container_add_component(&window->containers[1], line_component);

  // list init
  gui_container_info_init(bstate->list_container, bstate,
                          bstate->current_directory->files_count, 5,
                          bstate->font->size / 2 + 3);
  bstate->list_container->width = bstate->gui->size.x - 20;
  bstate->list_container->height = bstate->gui->size.y - bstate->list_container->y - 20;

  // commands register
  gui_list_commands_register(bstate->gui->commands, &bstate->click_state);
  commands_register(bstate->gui->commands, IN_KEYBOARD, 'e',
                    command_handler_exit, state);
  commands_register(bstate->gui->commands, IN_KEYBOARD, 'c',
                    command_handler_contextmenu, state);
  commands_register(bstate->gui->commands, IN_KEYBOARD, KEYBOARD_JUMP_RIGHT,
                    command_handler_jump_right, state);
  commands_register(bstate->gui->commands, IN_KEYBOARD, KEYBOARD_JUMP_LEFT,
                    command_handler_jump_left, state);

  return state;
}

static void browser_window_handle_folder_clicked(browser_window_state_t *bstate, file_t *current_file, logger_t *logger) {
    rgb_led_set_timeout(bstate->gui->pheripherals->rgb_leds, LED_LEFT, 0, 100,
                        100, 300);
    rgb_led_set_timeout(bstate->gui->pheripherals->rgb_leds, LED_RIGHT, 0, 100,
                        100, 300);

    char new_dir_path[path_join_memory_size(bstate->current_directory->path, current_file->name)];
    path_join(bstate->current_directory->path, current_file->name, new_dir_path);

    directory_or_error_t data = fileaccess_directory_list(bstate->state, new_dir_path);
    if (data.error) {
      fileaccess_log_error(logger, data.payload.error);
      dialog_info_show(bstate->gui, bstate->font, "Could not open directory",
                       fileaccess_get_error_text(data.payload.error));
    } else {
      fileaccess_directory_close(bstate->state, bstate->current_directory);
      bstate->current_directory = data.payload.directory;
      bstate->text_state.line = bstate->current_directory->path;

      gui_list_container_set_state(bstate->list_container, bstate, bstate->current_directory->files_count);

      logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Opening directory %s", bstate->current_directory->path);
    }
}

static void browser_window_item_clicked(container_t *container, void *state,
                                        uint32_t selected_index) {

  browser_window_state_t *bstate = (browser_window_state_t *)state;
  if (bstate->gui->active_window != bstate->browser_window) {
    return;
  }

  logger_t *logger = bstate->gui->logger;

  file_t current_file = bstate->current_directory->files[selected_index];

  if (current_file.type == FT_FILE) {
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__, "Opening file %s",
                current_file.name);
    opened_file_state_t opened = file_open(&current_file, browser_exec_options, bstate->state);
    file_browser_handle_opened_file(opened, bstate->gui, bstate->font);

  } else if (current_file.type == FT_FOLDER || current_file.type == FT_OTHER) {
    browser_window_handle_folder_clicked(bstate, &current_file, logger);
  }
}

static bool browser_window_running(void *state) {
  browser_window_state_t *bstate = (browser_window_state_t*)state;
  return bstate->running;
}

static void browser_window_job(void *state) {
  browser_window_state_t *bstate = (browser_window_state_t *)state;

  table_update_widths(&bstate->table, bstate->font,
                      bstate->current_directory->files, sizeof(file_t),
                      bstate->current_directory->files_count);

  bstate->line_component->y = bstate->font->size + 5;
  bstate->list_container->y = bstate->line_component->y / 2;
  gui_list_container_set_item_height(bstate->list_container,
                                     bstate->font->size);

  gui_list_ledstrip_update(bstate->list_container, bstate->gui->pheripherals);

  if (!bstate->running) {
    // cleanup
    fileaccess_directory_close(bstate->state, bstate->current_directory);
  }
}

