#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "gui.h"
#include "display_utils.h"
#include "font.h"

struct renderer_t {
  display_t *display;

  uint16_t translate_x;
  uint16_t translate_y;

  uint16_t render_area_width;
  uint16_t render_area_height;
};

renderer_t renderer_create(display_t *display);

void renderer_clear(renderer_t *renderer);
void renderer_render(renderer_t *renderer);

size2d_t renderer_write_string(renderer_t *renderer, uint16_t x, uint16_t y,
                               uint16_t length, font_t *font, char *text, display_pixel_t color);

size2d_t renderer_write_char(renderer_t *renderer, uint16_t x, uint16_t y,
                             font_t *font, char c, display_pixel_t color);

void renderer_render_rectangle(renderer_t *renderer, uint16_t x, uint16_t y,
                               uint16_t width, uint16_t height, display_pixel_t color);

void renderer_render_border(renderer_t *renderer, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height, display_pixel_t color);

void renderer_translate(renderer_t *renderer, uint16_t x, uint16_t y);
void renderer_clear_translate(renderer_t *renderer);

void renderer_set_draw_area(renderer_t *renderer, uint16_t width, uint16_t height);
void renderer_reset_draw_area(renderer_t *renderer);

#endif // __RENDERER_H__
