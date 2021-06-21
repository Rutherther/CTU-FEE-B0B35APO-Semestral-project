#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t font_bits_t;

typedef struct {
  const font_bits_t bits[256*16];
  const uint8_t width;
} font_character_t;

typedef struct {
  char *name;

  uint16_t height;
  uint16_t baseline;

  uint16_t first_char;

  uint16_t chars_count;
  const font_character_t *chars;

  uint16_t default_char;
} font_descriptor_t;

typedef struct {
  font_descriptor_t font;
  uint16_t size;

  uint16_t line_spacing;
  uint16_t char_spacing;
} font_t;

typedef struct {
  uint16_t x;
  uint16_t y;
} coords_t;

typedef coords_t size2d_t;

/**
 * @brief Create font from given descriptor
 * 
 * @param descriptor 
 * @return font_t 
 */
font_t font_create(font_descriptor_t descriptor);

/**
 * @brief Get text dimensions for given font 
 * 
 * @param font 
 * @param text 
 * @return size2d_t 
 */
size2d_t font_measure_text(font_t *font, char *text);

/**
 * @brief Get font character for font from char
 * 
 * @param font 
 * @param c 
 * @return font_character_t 
 */
font_character_t font_get_character(font_t *font, char c);

/**
 * @brief Get whether font contains font character 
 * 
 * @param font 
 * @param c 
 * @return true character is in font 
 * @return false character is not in font
 */
bool font_contains_character(font_t *font, char c);

/**
 * @brief Fit text with ellipsis to get how many characters can be shown
 * 
 * @param font 
 * @param size 
 * @param text 
 * @param ellipsis 
 * @return uint16_t number of characters that can be fit
 */
uint16_t font_fit_ellipsis(font_t *font, size2d_t size, char *text, char *ellipsis);

/**
 * @brief Fit cut text without any ellipsis to get how many characters can be shown
 * 
 * @param font 
 * @param size 
 * @param text 
 * @return uint16_t 
 */
uint16_t font_fit_cut(font_t *font, size2d_t size, char *text);

extern font_descriptor_t font_rom8x16;

#endif // __FONT_H__
