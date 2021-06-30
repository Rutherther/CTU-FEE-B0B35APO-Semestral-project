#ifndef __DISPLAY_UTILS_H__
#define __DISPLAY_UTILS_H__

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_MAX_RED 31u
#define DISPLAY_MAX_GREEN 63u
#define DISPLAY_MAX_BLUE 31u

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

typedef union {
  struct {
    uint8_t b : 5;
    uint8_t g : 6;
    uint8_t r : 5;
  } __attribute__((__packed__)) fields;
  uint16_t bits;
} __attribute__((__packed__)) display_pixel_t;

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} __attribute__((__packed__)) raw_pixel_onebit_t;

typedef struct {
  uint16_t red;
  uint16_t green;
  uint16_t blue;
} __attribute__((__packed__)) raw_pixel_t;

typedef struct {
  void* base_address;
} display_data_t;

typedef struct {
  display_pixel_t pixels[DISPLAY_HEIGHT * DISPLAY_WIDTH];
  display_data_t data;
} display_t;

extern const display_pixel_t BLACK_PIXEL;
extern const display_pixel_t WHITE_PIXEL;

extern const raw_pixel_t DISPLAY_PIXEL_MAX;

/**
 * @brief Make display_pixel_t from raw_pixel_onebit_t
 * 
 * @param pixel pixel to convert
 * @param max maximum rgb values (brightness) of raw pixel to calculate display pixel value from
 * @return display_pixel_t 
 */
display_pixel_t raw_pixel_onebit_convert_to_display(raw_pixel_onebit_t pixel,
                                             raw_pixel_onebit_t max);

/**
 * @brief Make display_pixel_t from raw_pixel_t
 * 
 * @param pixel pixel to convert
 * @param max maximum rgb values (brightness) of raw pixel to calculate display pixel value from
 * @return display_pixel_t 
 */
display_pixel_t raw_pixel_convert_to_display(raw_pixel_t pixel, raw_pixel_t max);

/**
 * @brief Call initialize display sequence and return its data
 * @note With COMPUTER macro this will initialize SDL window
 * 
 * @param data Data along with memory address where display is stored
 * @return display_t 
 */
display_t display_init(display_data_t data);

/**
 * @brief Clear the screen and call destroy sequence
 * @note With COMPUTER macro this will close SDL window
 * 
 * @param display 
 */
void display_deinit(display_t *display);

/**
 * @brief Render data in display_t structure on the display
 * @note With COMPUTER macro this will render data on SDL window
 * 
 * @param display 
 */
void display_render(display_t *display);

/**
 * @brief Clear display data in memory and optionally render it
 * 
 * @param display
 * @param render whether to render or only clear data in memory 
 */
void display_clear(display_t *display, bool render);

/**
 * @brief Return display pixel set in memory on given position
 * 
 * @param display 
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @return display_pixel_t pixel on given position
 */
display_pixel_t display_get_pixel(display_t *display, uint16_t x, uint16_t y);

/**
 * @brief Set pixel in memory on given position
 * 
 * @param display 
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param pixel pixel to set
 */
void display_set_pixel(display_t *display, uint16_t x, uint16_t y,
                       display_pixel_t pixel);

#endif // __DISPLAY_UTILS_H__
