#ifndef __MZAPO_SDL_H__
#define __MZAPO_SDL_H__

#include <stdint.h>

#define RGB_LEDS_COUNT 2

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} __attribute__((__packed__)) mzapo_sdl_rgb_led;

typedef struct {
  mzapo_sdl_rgb_led left;
  uint8_t offset;
  mzapo_sdl_rgb_led right;
} __attribute__((__packed__)) mzapo_sdl_rgb_leds;

typedef uint32_t mzapo_sdl_ledstrip;

typedef void (*display_command_fn)(uint16_t cmd);
typedef void (*display_data_fn)(uint16_t data);

typedef struct {
  display_command_fn cmd;
  display_data_fn data;
} __attribute__((__packed__)) mzapo_sdl_display;

typedef struct {
  uint32_t offset1;
  
  mzapo_sdl_ledstrip ledstrip;
  uint64_t offset2;
  mzapo_sdl_rgb_leds leds;
  uint8_t offset3;
  uint32_t reg_kbdrd_direct;
  uint64_t offset4;
  uint32_t reg_knobs_direct;
  uint32_t reg_knobs_8bit;
} __attribute__((__packed__)) mzapo_sdl_knobs_keyboard_rgb;

void mzapo_sdl_init();
void mzapo_sdl_deinit();

void *mzapo_sdl_map_phys(uint64_t region_base, uint64_t region_size);

#endif // __MZAPO_SDL_H__
