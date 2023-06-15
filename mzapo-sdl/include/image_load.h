#ifndef __IMAGE_LOAD_H__
#define __IMAGE_LOAD_H__

#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} __attribute__((__packed__)) buffer_pixel_t;

typedef struct {
  uint32_t width;
  uint32_t height;
  buffer_pixel_t *data;
} image_t;

image_t image_load_png(char *path);

#endif // __IMAGE_LOAD_H__
