#include "image.h"
#include "direction.h"
#include "display_utils.h"
#include <stdlib.h>



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

double get_scale_factor(uint16_t w, uint16_t h, image_region_t display_region) {
  double scale_x = (double)display_region.width / (double)w;
  double scale_y = (double)display_region.height / (double)h;

  double max = scale_x > scale_y ? scale_x : scale_y;
  double min = scale_x <= scale_y ? scale_x : scale_y;
  double scale = max;

  if (w * max > display_region.width || h * max > display_region.height) {
    scale = min;
  }

  return scale;
}

static void image_write_downscale(image_t *image, display_t *display,
                                  image_region_t region, image_region_t display_region, double scale_factor) {
  float downscale_factor = 1 / scale_factor;
  const uint16_t downscale_precision = 10000;
  uint32_t downscale_factor_i = downscale_factor * downscale_precision;
  uint16_t w = region.width, h = region.height;
  uint16_t sw = (uint16_t)(scale_factor * w), sh = (uint16_t)(scale_factor * h);

  unsigned long avg_range = ((unsigned long)downscale_factor);
  if (scale_factor - avg_range >= 0.5) {
    avg_range++;
  }

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

double image_write_to_display(image_t *image, display_t *display,
                              image_region_t region, image_region_t display_region) {
  display_clear(display, false);
  uint16_t w = region.width, h = region.height;
  if (w == display_region.width && h == display_region.height) {
    // write directly to image
    image_write_direct(image, display, region, display_region);
    return 1;
  }

  double scale = get_scale_factor(w, h, display_region);

  // scaling
  if (scale < 1) {
    image_write_downscale(image, display, region, display_region, scale);
  } else {
    image_write_upscale(image, display, region, display_region, scale);
  }

  return scale;
}
