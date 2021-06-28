#include "font.h"
#include <string.h>
#include <stdio.h>

static bool font_descriptor_contains_character(font_descriptor_t **descriptor,
                                               uint32_t c);


static int32_t absolute(int32_t a) {
  return a < 0 ? -a : a;
}

font_descriptor_t *font_family_get_descriptor(font_t *font) {
  if (font->family == NULL) {
    return &font->font;
  }


  int32_t nearest = font->family->descriptors[0]->height, nearest_index = 0;
  for (int i = 1; i < font->family->descriptors_count; i++) {
    font_descriptor_t *descriptor = font->family->descriptors[i];

    int32_t diff = absolute(descriptor->height - font->size);
    int32_t nearest_diff = absolute(nearest - font->size);

    if (diff < nearest_diff) {
      nearest_index = i;
      nearest = descriptor->height;
    }
  }

  printf("%d\r\n", nearest_index);
  return font->family->descriptors[nearest_index];
}

uint32_t font_get_real_char(char *text, uint16_t *bytes) {
  *bytes = 1;
  uint8_t first_byte_offset = 0;

  if ((*text & 0xC0) == 0xC0 && (*text & 0x20) == 0) {
    first_byte_offset = 3;
    *bytes = 2;
  }

  else if ((*text & 0xE0) == 0xE0 && (*text & 0x10) == 0) {
    first_byte_offset = 4;
    *bytes = 3;
  }

  else if ((*text & 0xF0) == 0xF0 && (*text & 0x8) == 0) {
    first_byte_offset = 5;
    *bytes = 4;
  }

  else {
    *bytes = 1;
    return (uint8_t)*(uint8_t*)text;
  }

  uint32_t result = 0;
  result |= *(uint8_t*)text;
  uint32_t mask = 0;

  for (int i = 0; i < 7 - first_byte_offset; i++) {
    mask |= 1 << i;
  }
  result &= mask;

  for (int i = 1; i < *bytes; i++) {
    result <<= 6;
    uint8_t current = *(text + i);

    if ((current & 0x80) != 0x80 || (current & 0x40)) {
      // malformed or no unicode, abort
      *bytes = 1;
      return (uint8_t)*(uint8_t*)text;
    }

    result |= current & 0x3F;
  }

  return result;  
}

font_t font_create(font_descriptor_t descriptor) {
  font_t font = {
      .font = descriptor,
      .size = descriptor.height,
      .char_spacing = 0,
      .line_spacing = 0,
      .family = NULL,
  };

  return font;
}

font_t font_family_create(font_descriptor_t def, font_family_t *family) {
  font_t font = {
    .font = def,
    .size = family->descriptors[0]->height,
    .char_spacing = 0,
    .line_spacing = 0,
    .family = family,
  };

  return font;
}

size2d_t font_measure_text(font_t *font, char *text) {
  size2d_t size = {.x = 0, .y = font->size};

  double scale = (double)font->size / font->font.height;
  size_t len = strlen(text);
  for (int i = 0; i < len && *text != '\0'; i++) {
    uint16_t bytes;
    uint32_t c = font_get_real_char(text, &bytes);
    text += bytes;

    font_character_t character = font_get_character(font, c);
    size.x += character.width * scale;
  }

  return size;
}

font_character_t font_get_character(font_t *font, uint32_t c) {
  font_descriptor_t *descriptor = font_family_get_descriptor(font);

  if (!font_descriptor_contains_character(&descriptor, c)) {
    return font_get_character(font, font->font.default_char);
  }

  uint32_t index = c - descriptor->first_char;
  uint16_t width = descriptor->max_width;
  if (descriptor->widths != NULL) {
    width = descriptor->widths[index];
  }

  uint32_t one_char_width =
      (width + sizeof(font_bits_t) * 8 - 1) / (sizeof(font_bits_t) * 8);
  uint32_t offset = (one_char_width * descriptor->height) * (index);
  if (descriptor->offsets != NULL) {
    offset = descriptor->offsets[index];
  }

  font_character_t character = {
    .width = width, .bits = descriptor->bits + offset};
  return character;
}

static bool font_descriptor_contains_character(font_descriptor_t **descriptor,
                                               uint32_t c) {
  if (*descriptor == NULL) {
    return false;
  }

  bool contains = c >= (*descriptor)->first_char &&
                  c - (*descriptor)->first_char < (*descriptor)->chars_count;

  if (!contains) {
    *descriptor = (*descriptor)->font_next_part;
    return font_descriptor_contains_character(descriptor, c);
  }

  return true;
}

bool font_contains_character(font_t *font, uint32_t c){
  font_descriptor_t *descriptor = font_family_get_descriptor(font);
  return font_descriptor_contains_character(&descriptor, c);
}

uint16_t
    font_fit_ellipsis(font_t *font, size2d_t size, char *text, char *ellipsis) {
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
