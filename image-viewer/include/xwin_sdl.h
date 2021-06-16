#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <stdbool.h>
#include <SDL.h>
#include <stdint.h>

int xwin_init(uint16_t w, uint16_t h);
void xwin_close();
void xwin_redraw(uint16_t w, uint16_t h, uint8_t *img);
bool xwin_poll_event(SDL_Event *event);
void xwin_save_image(char *output_name);

#endif
