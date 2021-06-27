#include "font.h"
#include <string.h>

font_t font_create(font_descriptor_t descriptor) {
  font_t font = {
    .font = descriptor,
    .size = descriptor.height,
    .char_spacing = 0,
    .line_spacing = 0,
  };

  return font;
}

size2d_t font_measure_text(font_t *font, char *text) {
  size2d_t size = {
    .x = 0,
    .y = font->size
  };

  double scale = (double)font->size / font->font.height;
  size_t len = strlen(text);
  for (int i = 0; i < len; i++) {
    font_character_t character = font_get_character(font, text[i]);
    size.x += character.width * scale;
  }

  return size;
}

font_character_t font_get_character(font_t *font, char c) {
  int16_t index = c - font->font.first_char;
  if (index < 0 || index >= font->font.chars_count) {
    index = font->font.default_char - font->font.first_char;
  }

  return font->font.chars[index];
}

bool font_contains_character(font_t *font, char c) {
  return c >= font->font.first_char && c - font->font.first_char < font->font.chars_count;
}

uint16_t font_fit_ellipsis(font_t *font, size2d_t size, char *text,
                           char *ellipsis) {
  uint16_t ellipsis_width = font_measure_text(font, ellipsis).x;
  size.x -= ellipsis_width;

  return font_fit_cut(font, size, text);
}

uint16_t font_fit_cut(font_t *font, size2d_t size, char *text) {
  size_t len = strlen(text);

  uint16_t x_size = 0;
  double scale = (double)font->size / font->font.height;
  for (int i = 0; i < len; i++) {
    font_character_t character = font_get_character(font, text[i]);
    x_size += character.width * scale;

    if (x_size > size.x) {
      return i;
    }
  }

  return 0;
}
