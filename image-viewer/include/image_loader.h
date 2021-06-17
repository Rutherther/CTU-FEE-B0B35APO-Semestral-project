#ifndef __IMAGE_LOADER_H__
#define __IMAGE_LOADER_H__

#include "image.h"

typedef void (*image_load_callback)(void *state, double process);

image_error_t image_loader_load(image_t *image, image_load_callback callback, void *state);

image_error_t image_loader_load_ppm(image_t *image,
                                    image_load_callback callback, void *state);
image_error_t image_loader_load_jpeg(image_t *image,
                                     image_load_callback callback, void *state);
image_error_t image_loader_load_png(image_t *image,
                                    image_load_callback callback, void *state);

image_error_t image_deduce_type(image_t *image);

#endif // __IMAGE_LOADER_H__
