#include <stdint.h>
#include "image.h"
#include "display_utils.h"

typedef struct {
  int32_t x;
  int32_t y;
} coords_t;

coords_t coords_create(int32_t x, int32_t y);
coords_t coords_get_image_screen_beg_coord(
    image_t *image, image_zoom_t zoom);
coords_t coords_get_image_screen_end_coords(image_t *image, image_zoom_t zoom);

coords_t image_get_screen_coords(image_t *image, image_zoom_t zoom, coords_t image_coords);
coords_t image_get_image_coords(image_t *image, image_zoom_t zoom, coords_t screen_coords);
