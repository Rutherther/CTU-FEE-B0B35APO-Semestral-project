#include "coords.h"

coords_t coords_create(int32_t x, int32_t y) {
  coords_t coords = {
    .x = x,
    .y = y
  };

  return coords;
}

coords_t coords_get_image_screen_beg_coord(image_t *image, image_zoom_t zoom) {
  uint16_t scaled_w = (uint16_t)(zoom.scale * image->width),
           scaled_h = (uint16_t)(zoom.scale * image->height);

  int32_t beg_x = ((double)DISPLAY_WIDTH - (double)(scaled_w)) / 2;
  int32_t beg_y = ((double)DISPLAY_HEIGHT - (double)(scaled_h)) / 2;

  if (beg_x < 0) {
    beg_x = 0;
  }

  if (beg_y < 0) {
    beg_y = 0;
  }

  return coords_create(beg_x, beg_y);
}

coords_t coords_get_image_screen_end_coords(image_t *image, image_zoom_t zoom) {
  uint16_t scaled_w = (uint16_t)(zoom.scale * image->width),
           scaled_h = (uint16_t)(zoom.scale * image->height);

  int32_t end_x = ((double)DISPLAY_WIDTH + (double)(scaled_w)) / 2;
  int32_t end_y = ((double)DISPLAY_HEIGHT + (double)(scaled_h)) / 2;

  if (end_x > DISPLAY_WIDTH - 1) {
    end_x = DISPLAY_WIDTH - 1;
  }

  if (end_y > DISPLAY_HEIGHT - 1) {
    end_y = DISPLAY_HEIGHT - 1;
  }

  return coords_create(end_x, end_y);
}

coords_t image_get_screen_coords(image_t *image, image_zoom_t zoom,
                                 coords_t image_coords) {
  coords_t beg = coords_get_image_screen_beg_coord(image, zoom);
  coords_t end = coords_get_image_screen_end_coords(image, zoom);

  image_region_t image_region =
      image_get_zoom_region(image, zoom);
  double relative_x = (double)(image_coords.x - (int32_t)image_region.x) / image_region.width;
  double relative_y = (double)(image_coords.y - (int32_t)image_region.y) / image_region.height;

  coords_t screen_coords = coords_create(beg.x + relative_x * (end.x - beg.x),
                                         beg.y + relative_y * (end.y - beg.y));
  return screen_coords;
}

coords_t image_get_image_coords(image_t *image, image_zoom_t zoom,
                                       coords_t screen_coords) {
  coords_t beg = coords_get_image_screen_beg_coord(image, zoom);
  coords_t end = coords_get_image_screen_end_coords(image, zoom);

  image_region_t image_region =
      image_get_zoom_region(image, zoom);
  double relative_x = (double)(screen_coords.x - beg.x) / (end.x - beg.x);
  double relative_y = (double)(screen_coords.y - beg.y) / (end.y - beg.y);

  coords_t image_coords =
      coords_create(image_region.x + relative_x * image_region.width,
                    image_region.y + relative_y * image_region.height);
  return image_coords;
}
