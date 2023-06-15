#ifndef __SDL_UTILS_H__
#define __SDL_UTILS_H__

#include "image_load.h"
#include <stdint.h>

#define DISPLAY_WIDTH 480
#define DISPLAY_HEIGHT 320

#define WINDOW_WIDTH 760
#define WINDOW_HEIGHT 1000

#define LCD_START_X 114
#define LCD_START_Y 346

#define RGB_LEFT_LED_START_X 136
#define RGB_LEFT_LED_START_Y 817

#define RGB_RIGHT_LED_START_X 587
#define RGB_RIGHT_LED_START_Y 811

#define RGB_LED_WIDTH 25
#define RGB_LED_HEIGHT 25

#define LEDSTRIP_START_X 103
#define LEDSTRIP_START_Y 968

#define LEDSTRIP_WIDTH 10
#define LEDSTRIP_HEIGHT 10

#define LEDSTRIP_STEP_X 16
#define LEDSTRIP_STEP_Y -5.0/31.0

extern buffer_pixel_t buffer[WINDOW_WIDTH * WINDOW_HEIGHT];

void sdl_utils_init();
void sdl_utils_deinit();

void sdl_utils_render_buffer();

void sdl_utils_clear_buffer();

void sdl_utils_draw_image();
void sdl_utils_draw_ledstrip(uint32_t values);
void sdl_utils_draw_display(uint16_t *buffer);
void sdl_utils_draw_rgb_leds(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2);

#endif // __SDL_UTILS_H__
