#include "image.h"
#include "direction.h"
#include "display_utils.h"
#include "logger.h"
#include <stdlib.h>

char *image_error_strings[] = {
    "Success",        "Unknown format",   "No such file or directory",
    "No permissions", "Cannot open file", "Wrong format",
    "Unknown"};

void image_error_log(logger_t *logger, image_error_t error) {
  logger_error(logger, __FILE__, __FUNCTION__, __LINE__, "There was an error loading image: %s", image_error_strings[error]);
}

image_t image_create(char *path) {
  image_t image = {
    .path = path,
    .width = 0,
    .height = 0,
    .pixels = NULL,
    .type = IMG_UNKNOWN,
  };

  return image;
}

void image_destroy(image_t *image) {
  if (image->pixels == NULL) {
    free(image->pixels);
  }
}

image_region_t image_region_create(uint16_t x, uint16_t y, uint16_t width,
                                   uint16_t height) {
  image_region_t region = {
    .x = x,
    .y = y,
    .width = width,
    .height = height,
  };

  return region;
}

bool image_region_move_within(image_region_t *to_move, direction_t direction,
                              int amount, image_region_t *border) {
  int32_t x = to_move->x;
  int32_t y = to_move->y;
  direction_move_xy(direction, &x, &y, amount);

  if (x < border->x) {
    x = border->x;
  } else if (x + to_move->width >= border->width + border->x) {
    x = border->x + border->width - 1 - to_move->width;
  }

  if (y < border->y) {
    y = border->y;
  } else if (y + to_move->height >= border->height + border->y) {
    y = border->x + border->height - 1 - to_move->height;
  }

  bool changed = to_move->x != x || to_move->y != y;
  to_move->x = x;
  to_move->y = y;

  return changed;
}

display_pixel_t image_get_pixel(image_t *image, uint16_t x, uint16_t y) {
  if (y >= image->height || x >= image->width) {
    return BLACK_PIXEL;
  }
  return image->pixels[y * image->width + x];
}

void image_set_pixel(image_t *image, uint16_t x, uint16_t y,
                       display_pixel_t pixel) {
  if (y >= image->height || x >= image->width) {
    return;
  }

  image->pixels[y * image->width + x] = pixel;
}

static void image_write_downscale(image_t *image, display_t *display,
                                  image_region_t region, image_region_t display_region, double scale_factor) {
  float downscale_factor = 1 / scale_factor;
  const uint16_t downscale_precision = 10000;
  uint32_t downscale_factor_i = downscale_factor * downscale_precision;
  uint16_t w = region.width, h = region.height;
  uint16_t sw = (uint16_t)(scale_factor * w), sh = (uint16_t)(scale_factor * h);

  uint16_t beg_x = (display_region.width - sw) / 2 + display_region.x;
  uint16_t beg_y = (display_region.height - sh) / 2 + display_region.y;

  for (int y = 0; y < sh; y++) {
    for (int x = 0; x < sw; x++) {
      uint16_t px = ((uint64_t)(downscale_factor_i * (2*x + 1)))/(downscale_precision * 2) + region.x;
      uint16_t py = ((uint64_t)(downscale_factor_i * (2*y + 1)))/(downscale_precision * 2) + region.y;
      display_pixel_t result_display = image_get_pixel(image, px, py);
      display_set_pixel(display, x + beg_x, y + beg_y, result_display);
    }
  }
}

static void image_write_upscale(image_t *image, display_t *display,
                                image_region_t region, image_region_t display_region, double scale_factor) {
  float downscale_factor = 1 / scale_factor;
  const uint16_t downscale_precision = 10000;
  uint32_t downscale_factor_i = downscale_factor * downscale_precision;
  uint16_t w = region.width, h = region.height;
  uint16_t sw = (uint16_t)(scale_factor * w), sh = (uint16_t)(scale_factor * h);

  uint16_t beg_x = (display_region.width - sw) / 2 + display_region.x;
  uint16_t beg_y = (display_region.height - sh) / 2 + display_region.y;

  for (int y = 0; y < sh; y++) {
    for (int x = 0; x < sw; x++) {
      uint16_t px = (downscale_factor_i * (2*x + 1))/(downscale_precision * 2) + region.x;
      uint16_t py = (downscale_factor_i * (2*y + 1))/(downscale_precision * 2) + region.y;

      display_pixel_t result_display = image_get_pixel(image, px, py);
      display_set_pixel(display, x + beg_x, y + beg_y, result_display);
    }
  }
}

static void image_write_direct(image_t *image, display_t *display,
                               image_region_t region, image_region_t display_region) {
  for (int y = 0; y < region.height; y++) {
    for (int x = 0; x < region.width; x++) {
      display_set_pixel(display, x + display_region.x, y + display_region.y, image_get_pixel(image, x + region.x, y + region.y));
    }
  }
}

double min(double a, double b) {
  return a < b ? a : b;
}

image_zoom_t image_get_initial_zoom(image_t *image) {
  double scale_x = (double)DISPLAY_WIDTH / image->width;
  double scale_y = (double)DISPLAY_HEIGHT / image->height;

  double scale = min(min(scale_x, scale_y), 1);

  image_zoom_t zoom = {
    .scale = scale,
    .x = 0,
    .y = 0,
  };

  return zoom;
}

image_region_t image_get_zoom_region(image_t *image, image_zoom_t zoom) {
  double fits_screen_width = DISPLAY_WIDTH / (double)(image->width * zoom.scale);
  double fits_screen_height =
      DISPLAY_HEIGHT / (double)(image->height * zoom.scale);

  uint16_t width = fits_screen_width * image->width;
  uint16_t height = fits_screen_height * image->height;

  if (fits_screen_width > 1) {
    width = image->width;
  }

  if (fits_screen_height > 1) {
    height = image->height;
  }

  image_region_t region = {
    .x = zoom.x,
    .y = zoom.y,
    .width = width,
    .height = height
  };

  if (region.x + region.width > image->width) {
    region.x = image->width - region.width;
  }

  if (region.y + region.height > image->height) {
    region.y = image->height - region.height;
  }

  return region;
}

image_zoom_t image_write_to_display(image_t *image, display_t *display,
                              image_zoom_t zoom) {
  display_clear(display, false);
  image_region_t region = image_get_zoom_region(image, zoom);
  zoom.x = region.x;
  zoom.y = region.y;

  image_region_t display_region = image_region_create(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  if (zoom.scale == 1) {
    // write directly to image
    image_write_direct(image, display, region, display_region);
    return zoom;
  }

  double scale = zoom.scale;

  // scaling
  if (scale < 1) {
    image_write_downscale(image, display, region, display_region, scale);
  } else {
    image_write_upscale(image, display, region, display_region, scale);
  }

  return zoom;
}
