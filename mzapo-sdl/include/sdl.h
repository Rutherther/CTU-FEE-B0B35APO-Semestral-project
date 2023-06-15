#ifndef __SDL_H__
#define __SDL_H__

#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>
#include <stdint.h>
#include <stdbool.h>

#include "SDL_surface.h"
#include "SDL_video.h"


int sdl_init(uint16_t w, uint16_t h);
void sdl_close();

void sdl_redraw(uint16_t w, uint16_t h, uint8_t *img);

bool sdl_poll_event(SDL_Event *event);
#endif // __SDL_H__
