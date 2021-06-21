#include <stdint.h>
#include "image.h"
#include "display_utils.h"

typedef struct {
  int32_t x;
  int32_t y;
} coords_t;

/**
 * @brief Create coords with given x and y coordinates
 * 
 * @param x x coordinate
 * @param y y coordinate
 * @return coords_t 
 */
coords_t coords_create(int32_t x, int32_t y);

/**
 * @brief Get begin coords on screen of given image and scale
 * 
 * @param image 
 * @param zoom 
 * @return coords_t 
 */
coords_t coords_get_image_screen_beg_coord(
    image_t *image, image_zoom_t zoom);

/**
 * @brief Get end coords on screen of given image and scale
 * 
 * @param image 
 * @param zoom 
 * @return coords_t 
 */
coords_t coords_get_image_screen_end_coords(image_t *image, image_zoom_t zoom);

/**
 * @brief Map image coords to screen coords for given image and zoom 
 * 
 * @param image 
 * @param zoom 
 * @param image_coords 
 * @return coords_t 
 */
coords_t image_get_screen_coords(image_t *image, image_zoom_t zoom, coords_t image_coords);

/**
 * @brief Map screen coords to image coords for given image and zoom
 * 
 * @param image 
 * @param zoom 
 * @param screen_coords 
 * @return coords_t 
 */
coords_t image_get_image_coords(image_t *image, image_zoom_t zoom, coords_t screen_coords);
