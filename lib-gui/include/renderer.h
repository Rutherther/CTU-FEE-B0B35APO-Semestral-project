#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "display_utils.h"
#include "font.h"
#include "gui.h"

struct renderer_t {
  display_t *display;

  uint16_t translate_x;
  uint16_t translate_y;

  uint16_t render_area_width;
  uint16_t render_area_height;
};

/**
 * @brief Create renderer for given display
 *
 * @param display
 * @return renderer_t
 */
renderer_t renderer_create(display_t *display);

/**
 * @brief Clear display data
 *
 * @param renderer
 */
void renderer_clear(renderer_t *renderer);

/**
 * @brief Render data to display
 *
 * @param renderer
 */
void renderer_render(renderer_t *renderer);

/**
 * @brief Render string on display char by char
 *
 * @param renderer
 * @param x begin x coord
 * @param y begin y coord
 * @param length length of the text to be rendered / or the whole text if 0 or
 * greater than text length
 * @param font
 * @param text
 * @param color
 * @return size2d_t
 */
size2d_t renderer_write_string(renderer_t *renderer, uint16_t x, uint16_t y,
                               uint16_t length, font_t *font, const char *text,
                               display_pixel_t color);

/**
 * @brief Render render one character
 *
 * @param renderer
 * @param x begin x coord
 * @param y begin y coord
 * @param font
 * @param c character
 * @param color
 * @return size2d_t
 */
size2d_t renderer_write_char(renderer_t *renderer, uint16_t x, uint16_t y,
                             font_t *font, uint32_t c, display_pixel_t color);

/**
 * @brief Render filled rectangle
 *
 * @param renderer
 * @param x base x coord
 * @param y base y coord
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color
 */
void renderer_render_rectangle(renderer_t *renderer, uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height,
                               display_pixel_t color);

/**
 * @brief Render rectangle border
 *
 * @param renderer
 * @param x base x coord
 * @param y base y coord
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color
 */
void renderer_render_border(renderer_t *renderer, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height,
                            display_pixel_t color);

/**
 * @brief Translate coordinate  system of the renderer
 *
 * @param renderer
 * @param x translate x coord
 * @param y translate y coord
 */
void renderer_translate(renderer_t *renderer, uint16_t x, uint16_t y);

/**
 * @brief Clear translation
 *
 * @param renderer
 */
void renderer_clear_translate(renderer_t *renderer);

/**
 * @brief Set draw area size so nothing is drawn outside
 *
 * @param renderer
 * @param width
 * @param height
 */
void renderer_set_draw_area(renderer_t *renderer, uint16_t width,
                            uint16_t height);

/**
 * @brief Reset draw area
 *
 * @param renderer
 */
void renderer_reset_draw_area(renderer_t *renderer);

#endif // __RENDERER_H__
