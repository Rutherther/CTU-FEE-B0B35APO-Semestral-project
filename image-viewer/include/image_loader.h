#ifndef __IMAGE_LOADER_H__
#define __IMAGE_LOADER_H__

#include "image.h"

typedef void (*image_load_callback)(void *state, double process);

/**
 * @brief Load image from any supported types
 * 
 * @param image 
 * @param callback 
 * @param state 
 * @return image_error_t 
 */
image_error_t image_loader_load(image_t *image, image_load_callback callback, void *state);

/**
 * @brief Load image from ppm format
 * 
 * @param image 
 * @param callback 
 * @param state 
 * @return image_error_t 
 */
image_error_t image_loader_load_ppm(image_t *image,
                                    image_load_callback callback, void *state);

/**
 * @brief Load image from jpeg format
 * 
 * @param image 
 * @param callback 
 * @param state 
 * @return image_error_t 
 */
image_error_t image_loader_load_jpeg(image_t *image,
                                     image_load_callback callback, void *state);

/**
 * @brief Load image from png
 * 
 * @param image 
 * @param callback 
 * @param state 
 * @return image_error_t 
 */
image_error_t image_loader_load_png(image_t *image,
                                    image_load_callback callback, void *state);

/**
 * @brief Deduce image type from magic number
 * 
 * @param image 
 * @return image_error_t 
 */
image_error_t image_deduce_type(image_t *image);

#endif // __IMAGE_LOADER_H__
