#include "image.h"
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

bool image_write_to_display(image_t *image, display_t *display,
                            image_region_t region) {
  uint16_t w = region.width, h = region.height;
  uint16_t x = region.x, y = region.y;

  image_region_t display_region = image_region_create(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  double scale = get_scale_factor(w, h, display_region);

  uint16_t sw = (uint16_t)(scale * w), sh = (uint16_t)(scale * h);

  // scaling
  return true;
}
