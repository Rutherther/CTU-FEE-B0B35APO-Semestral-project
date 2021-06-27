#include "renderer.h"
#include "display_utils.h"
#include "font.h"
#include <string.h>

renderer_t renderer_create(display_t *display) {
  renderer_t renderer = {
      .display = display, .translate_x = 0, .translate_y = 0};

  return renderer;
}

static coords_t renderer_get_end_coords(renderer_t *renderer) {
  coords_t coords = {
      .x = renderer->render_area_width,
      .y = renderer->render_area_height,
  };

  if (renderer->render_area_width == 0 && renderer->render_area_height == 0) {
    coords.x = coords.y = 0;
  }

  return coords;
}

static coords_t coords_translate(renderer_t *renderer, int16_t x, int16_t y) {
  coords_t coords = {
      .x = renderer->translate_x + x,
      .y = renderer->translate_y + y,
  };

  return coords;
}

static coords_t renderer_get_beg_coords(renderer_t *renderer) {
  coords_t coords = {
      .x = 0,
      .y = 0,
  };

  return coords;
}

static bool coords_is_within(int16_t x, int16_t y, coords_t start,
                             coords_t end) {
  if (end.x == 0 && end.y == 0) { // default for no area
    return true;
  }

  return x >= start.x && y >= start.y && x <= end.x && y <= end.y;
}

void renderer_clear(renderer_t *renderer) {
  display_clear(renderer->display, false);
}

void renderer_render(renderer_t *renderer) {
  display_render(renderer->display);
}

size2d_t renderer_write_string(renderer_t *renderer, uint16_t bx, uint16_t by,
                               uint16_t length, font_t *font, char *text,
                               display_pixel_t color) {
  uint16_t x = bx, y = by;
  size_t len = strlen(text);

  if (length != 0 && length < len) {
    len = length;
  }

  for (int i = 0; i < len; i++) {
    font_character_t character = font_get_character(font, text[i]);
    renderer_write_char(renderer, x, y, font, text[i], color);
    x += character.width + font->char_spacing;
  }

  size2d_t size = {.x = x - bx, .y = font->size + font->line_spacing};

  return size;
}

size2d_t renderer_write_char(renderer_t *renderer, uint16_t x, uint16_t y,
                             font_t *font, char c, display_pixel_t color) {
  coords_t beg = renderer_get_beg_coords(renderer);
  coords_t end = renderer_get_end_coords(renderer);

  font_character_t character = font_get_character(font, c);
  for (int i = 0; i < font->font.height; i++) {
    font_bits_t line_bits = character.bits[i]; // line

    for (int j = 0; j < character.width; j++) {
      bool current = (line_bits >> (8*sizeof(font_bits_t) - j - 1)) & 1;

      if (current && coords_is_within(x + j, y + i, beg, end)) {
        coords_t translated = coords_translate(renderer, x + j, y + i);
        display_set_pixel(renderer->display, translated.x, translated.y, color);
      }
    }
  }

  size2d_t size = {.x = character.width + font->char_spacing,
                   .y = font->size + font->line_spacing};
  return size;
}

void renderer_render_rectangle(renderer_t *renderer, uint16_t bx, uint16_t by,
                               uint16_t width, uint16_t height,
                               display_pixel_t color) {
  coords_t beg = renderer_get_beg_coords(renderer);
  coords_t end = renderer_get_end_coords(renderer);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (coords_is_within(bx + x, by + y, beg, end)) {
        display_set_pixel(renderer->display, bx + x + renderer->translate_x,
                          by + y + renderer->translate_y, color);
      }
    }
  }
}

void renderer_render_border(renderer_t *renderer, uint16_t x, uint16_t y,
                            uint16_t width, uint16_t height,
                            display_pixel_t color) {
  coords_t beg = renderer_get_beg_coords(renderer);
  coords_t end = renderer_get_end_coords(renderer);

  if (width > 1 || height == 1) {
    for (int i = 0; i < width; i++) {
      if (coords_is_within(x + i, y, beg, end)) {
        display_set_pixel(renderer->display, x + i, y, color);
      }

      if (coords_is_within(x + i, y + height, beg, end)) {
        display_set_pixel(renderer->display, x + i, y + height, color);
      }
    }
  }

  if (height > 1) {
    for (int i = 0; i < height; i++) {
      if (coords_is_within(x, y + i, beg, end)) {
        display_set_pixel(renderer->display, x, y + i, color);
      }

      if (coords_is_within(x + width, y + i, beg, end)) {
        display_set_pixel(renderer->display, x + width, y + i, color);
      }
    }
  }
}

void renderer_translate(renderer_t *renderer, uint16_t x, uint16_t y) {
  renderer->translate_x += x;
  renderer->translate_y += y;
}

void renderer_clear_translate(renderer_t *renderer) {
  renderer->translate_x = 0;
  renderer->translate_y = 0;
}

void renderer_set_draw_area(renderer_t *renderer, uint16_t width,
                            uint16_t height) {
  renderer->render_area_width = width;
  renderer->render_area_height = height;
}

void renderer_reset_draw_area(renderer_t *renderer) {

  renderer->render_area_height = 0;
  renderer->render_area_width = 0;
}
