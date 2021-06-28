#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>
#include <magic.h>
#include <png.h>
#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>

#include "direction.h"
#include "logger.h"
#include "display_utils.h"

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
  double scale;
} image_zoom_t;

typedef struct {
  uint16_t x;
  uint16_t y;

  uint16_t width;
  uint16_t height;
} image_region_t;

/**
 * @brief Create image with given path
 *
 * @param path path to image file
 * @return image_t
 */
image_t image_create(char *path);

void image_error_log(logger_t *logger, image_error_t error);

/**
 * @brief Clean up image data
 *
 * @param image
 */
void image_destroy(image_t *image);

/**
 * @brief Get image pixel on coords
 *
 * @param image
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @return display_pixel_t
 */
display_pixel_t image_get_pixel(image_t *image, uint16_t x, uint16_t y);

/**
 * @brief Set image pixel on coords
 *
 * @param image
 * @param x x coords of pixel
 * @param y y coords of pixel
 * @param pixel pixel to set
 */
void image_set_pixel(image_t *image, uint16_t x, uint16_t y,
                     display_pixel_t pixel);

/**
 * @brief Create image region
 *
 * @param x begin x coord
 * @param y begin y coord
 * @param width width of the region
 * @param height height of the region
 * @return image_region_t
 */
image_region_t image_region_create(uint16_t x, uint16_t y, uint16_t width,
                                   uint16_t height);
bool image_region_move_within(image_region_t *to_move, direction_t direction,
                              int amount, image_region_t *border);

/**
 * @brief Write image data to display data
 *
 * @param image
 * @param display
 * @param scale
 * @return image_zoom_t
 */
image_zoom_t image_write_to_display(image_t *image, display_t *display,
                                    image_zoom_t scale);

/**
 * @brief Get initial zoom to show whole image on the display
 *
 * @param image
 * @return image_zoom_t
 */
image_zoom_t image_get_initial_zoom(image_t *image);

/**
 * @brief Get shown region from image and zoom
 *
 * @param image
 * @param zoom
 * @return image_region_t
 */
image_region_t image_get_zoom_region(image_t *image, image_zoom_t zoom);

#endif // __IMAGE_H__
