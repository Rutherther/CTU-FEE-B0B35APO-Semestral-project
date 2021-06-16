/*
 * File name: xwin_sdl.c
 * Date:      2015/06/18 14:37
 * Author:    Jan Faigl
 */

#include <assert.h>

#include <SDL.h>
#include <SDL_image.h>

#include "SDL_surface.h"
#include "SDL_video.h"
#include "xwin_sdl.h"

static SDL_Window *win = NULL;

int xwin_init(uint16_t w, uint16_t h) {
  int r = 0;
  r = SDL_Init(SDL_INIT_VIDEO);
  assert(win == NULL);
  win = SDL_CreateWindow("APO Test program", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
  assert(win != NULL);
  SDL_SetWindowTitle(win, "APO Test program");
  SDL_Surface *surface = SDL_CreateRGBSurface(
                         32, 32, 24, 32 * 3, 0xff, 0xff00, 0xff0000, 0x0000);
  SDL_SetWindowIcon(win, surface);
  SDL_FreeSurface(surface);

  SDL_SetWindowResizable(win, SDL_FALSE);
  return r;
}

void xwin_close() {
  assert(win != NULL);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

void xwin_redraw(uint16_t w, uint16_t h, uint8_t *img) {
  assert(img && win);
  SDL_Surface *scr = SDL_GetWindowSurface(win);

  for (int y = 0; y < scr->h && y < h; ++y) {
    for (int x = 0; x < scr->w && x < w; ++x) {
      const int idx = (y * scr->w + x) * scr->format->BytesPerPixel;
      Uint8 *px = (Uint8 *)scr->pixels + idx;
      uint64_t position_in_img = (y * w + x) * 3;
      *(px + scr->format->Rshift / 8) = *(img + position_in_img);
      *(px + scr->format->Gshift / 8) = *(img + position_in_img + 1);
      *(px + scr->format->Bshift / 8) = *(img + position_in_img + 2);
    }
  }
  SDL_UpdateWindowSurface(win);
  SDL_FreeSurface(scr);
}

void xwin_save_image(char *output_name) {
  IMG_Init(IMG_INIT_PNG);
  SDL_Surface *scr = SDL_GetWindowSurface(win);
  IMG_SavePNG(scr, output_name);
  SDL_FreeSurface(scr);
  IMG_Quit();
}

bool xwin_poll_event(SDL_Event *event) {
  return SDL_PollEvent(event);
}

/* end of xwin_sdl.c */
