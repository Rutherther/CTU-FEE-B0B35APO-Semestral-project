#include "image_viewer_handlers.h"
#include "cursor.h"
#include "keyboard_const.h"
#include "rotation_const.h"

#define MAX_ZOOM 10
#define MIN_ZOOM 0.02


void image_viewer_display_image(image_viewer_t *viewer) {
  cursor_hide(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
  viewer->scale =
      image_write_to_display(&viewer->image, viewer->display, viewer->scale);
  display_render(viewer->display);
}

static void image_viewer_handle_moved_cursor(image_viewer_t *viewer,
                                             bool move_cursor,
                                             direction_t direction, int amount,
                                             int32_t x, int32_t y) {
  if (x < 0) {
    x = 0;
  }
  if (y < 0) {
    y = 0;
  }

  viewer->scale.x = x;
  viewer->scale.y = y;

  if (move_cursor) {
    cursor_move(&viewer->cursor,
                image_get_zoom_region(&viewer->image, viewer->scale), direction,
                amount / viewer->scale.scale);
  }

  image_viewer_display_image(viewer);
  cursor_show(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
  display_render(viewer->display);

  ledstrip_turn_on(
      &viewer->ledstrip,
      ((double)viewer->cursor.x / viewer->image.width) * LED_STRIP_COUNT, 1);
}

static void command_handler_move_cursor(void *data, direction_t direction, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;

  cursor_hide(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
  int32_t x = viewer->scale.x, y = viewer->scale.y;
  bool move_cursor = false;
  switch (viewer->mode) {
  case MOD_CURSOR:
    if (!cursor_move(&viewer->cursor,
                     image_get_zoom_region(&viewer->image, viewer->scale),
                     direction, amount / viewer->scale.scale)) {
      direction_move_xy(direction, &x,
                        &y,
                        amount / viewer->scale.scale);
      move_cursor = true;
    }
    break;
  case MOD_IMAGE:
    logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__, "Moving image");
    direction_move_xy(direction, &x,
                      &y,
                      amount / viewer->scale.scale);
    move_cursor = true;
    break;
  default:
    break;
  }

  image_viewer_handle_moved_cursor(viewer, move_cursor, direction, amount, x,
                                   y);
}


static void command_handler_move_left(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t*)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor to left by %d", amount);
  command_handler_move_cursor(data, LEFT, amount);
}

static void command_handler_move_right(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor to right by %d", amount);
  command_handler_move_cursor(data, RIGHT, amount);
}

static void command_handler_move_up(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor up by %d", amount);
  command_handler_move_cursor(data, UP, amount);
}

static void command_handler_move_down(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Moving cursor down by %d", amount);
  command_handler_move_cursor(data, DOWN, amount);
}


static void command_handler_exit(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  viewer->running = false;
}

static image_zoom_t zoom_get_new(image_viewer_t *viewer, int amount) {
  double zoom = viewer->scale.scale;
  double new_zoom = zoom * (1 + amount * 0.05);

  if (new_zoom > MAX_ZOOM) {
    logger_warn(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
                "Cannot zoom more than %f", MAX_ZOOM);
    new_zoom = MAX_ZOOM;
  }

  if (new_zoom < MIN_ZOOM) {
    logger_warn(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
                "Cannot zoom less than %f", MIN_ZOOM);
    new_zoom = MIN_ZOOM;
  }

  image_zoom_t tmp = {
      .scale = new_zoom,
      .x = viewer->scale.x,
      .y = viewer->scale.y,
  };
  return tmp;
}

static void zoom(image_viewer_t *viewer, int amount) {
  double zoom = viewer->scale.scale;
  image_region_t current_region = image_get_zoom_region(&viewer->image, viewer->scale);
  image_zoom_t tmp = zoom_get_new(viewer, amount);
  image_region_t new_region =
      image_get_zoom_region(&viewer->image, tmp);

  uint16_t scaled_w = (uint16_t)(zoom * viewer->image.width),
    scaled_h = (uint16_t)(zoom * viewer->image.height);

  int32_t beg_x = ((double)DISPLAY_WIDTH - (double)(scaled_w)) / 2;
  int32_t beg_y = ((double)DISPLAY_HEIGHT - (double)(scaled_h)) / 2;

  if (beg_x < 0) {
    beg_x = 0;
  }

  if (beg_y < 0) {
    beg_y = 0;
  }

  if (scaled_w > DISPLAY_WIDTH) {
    scaled_w = DISPLAY_WIDTH;
  }

  if (scaled_h > DISPLAY_HEIGHT) {
    scaled_h = DISPLAY_HEIGHT;
  }

  int32_t diff_w = current_region.width - new_region.width;
  int32_t diff_h = current_region.height - new_region.height;

  double side_percentage_x =
      (double)(viewer->cursor.x - current_region.x) / current_region.width;
  double side_percentage_y =
      (double)(viewer->cursor.y - current_region.y) / current_region.height;

  tmp.x += side_percentage_x * diff_w;
  tmp.y += side_percentage_y * diff_h;
  viewer->scale = tmp;

  image_viewer_display_image(viewer);
  cursor_show(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
}

static void command_handler_zoom_in(void *data, int amount) {
  // hide cursor, zoom, show cursor
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zooming in by %d", amount);

  zoom(viewer, amount);
}

static void command_handler_zoom_out(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zooming out by %d", amount);

  zoom(viewer, -amount);
}

static void command_handler_zoom_reset(void *data, int amount) {
  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Zoom reset by %d", amount);
  viewer->scale = image_get_initial_zoom(&viewer->image);
  image_viewer_display_image(viewer);
  cursor_center(&viewer->cursor, viewer->image_region);
  cursor_show(&viewer->cursor, &viewer->image, viewer->scale, viewer->display);
}

static void command_handler_change_mode(void *data, int amount) {
  if (!amount) {
    return;
  }

  image_viewer_t *viewer = (image_viewer_t *)data;
  logger_debug(viewer->logger, __FILE__, __FUNCTION__, __LINE__,
               "Changing mode");


  uint8_t mode = viewer->mode;
  mode++;
  if (mode >= MOD_COUNT) {
    mode %= MOD_COUNT;
  }

  if (mode == MOD_IMAGE) {
    rgb_led_set_green(&viewer->rgb_leds, LED_LEFT);
    rgb_led_set_green(&viewer->rgb_leds, LED_RIGHT);
  } else {
    rgb_led_clear(&viewer->rgb_leds, LED_LEFT);
    rgb_led_clear(&viewer->rgb_leds, LED_RIGHT);
  }

  viewer->mode = mode;
}

void image_viewer_register_commands(image_viewer_t *viewer, commands_t *commands) {
  commands_register(commands, IN_KEYBOARD, KEYBOARD_LEFT, &command_handler_move_left,
                    viewer);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_DOWN, &command_handler_move_down,
                    viewer);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_UP, &command_handler_move_up,
                    viewer);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_RIGHT, &command_handler_move_right,
                    viewer);

  commands_register(commands, IN_KEYBOARD, 'e', &command_handler_exit,
                    viewer);

  commands_register(commands, IN_KEYBOARD, KEYBOARD_ZOOM_IN, &command_handler_zoom_in,
                    viewer);
  commands_register(commands, IN_KEYBOARD, KEYBOARD_ZOOM_OUT, &command_handler_zoom_out,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'r', &command_handler_zoom_reset,
                    viewer);
  commands_register(commands, IN_KEYBOARD, 'm', &command_handler_change_mode,
                    viewer);

  commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_HORIZONTAL, &command_handler_move_right,
                    viewer);
  commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_VERTICAL,
                    &command_handler_move_down, viewer);
  commands_register(commands, IN_ENCODER_ROTATE, ROTATION_ENCODER_ZOOM, &command_handler_zoom_in,
                    viewer);

  commands_register(commands, IN_ENCODER_CLICK, 0, &command_handler_exit,
                    viewer);

  commands_register(commands, IN_ENCODER_CLICK, 1, &command_handler_change_mode,
                    viewer);

  commands_register(commands, IN_ENCODER_CLICK, 2, &command_handler_zoom_reset,
                    viewer);
}
