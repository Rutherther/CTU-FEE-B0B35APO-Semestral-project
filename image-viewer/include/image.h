#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <magic.h>
#include <stdio.h>
#include <jpeglib.h>
#include <png.h>

#include <stdbool.h>
#include <stdint.h>

#include "display_utils.h"
#include "direction.h"

typedef enum {
  IMG_UNKNOWN,
  IMG_PNG,
  IMG_JPG,
  IMG_PPM,
} image_type_t;

typedef enum {
  IMERR_SUCCESS,
  IMERR_UNKNOWN_FORMAT,
  IMERR_FILE_NOT_FOUND,
  IMERR_FILE_NO_PERMISSIONS,
  IMERR_FILE_CANT_OPEN,
  IMERR_WRONG_FORMAT,
  IMERR_UNKNOWN,
} image_error_t;

typedef struct {
  char *path;
  image_type_t type;

  display_pixel_t *pixels;

  uint16_t width;
  uint16_t height;
} image_t;

typedef struct {
  uint16_t x;
  uint16_t y;

  uint16_t width;
  uint16_t height;
} image_region_t;

image_t image_create(char *path);
void image_destroy(image_t *image);

display_pixel_t image_get_pixel(image_t *image, uint16_t x, uint16_t y);

void image_set_pixel(image_t *image, uint16_t x, uint16_t y,
                     display_pixel_t pixel);

image_region_t image_region_create(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
bool image_region_move_within(image_region_t *to_move, direction_t direction,
                              int amount, image_region_t *border);

double image_write_to_display(image_t *image, display_t *display, image_region_t region);

#endif // __IMAGE_H__
