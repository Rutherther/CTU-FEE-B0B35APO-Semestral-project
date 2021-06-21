#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <stdbool.h>
#include <SDL.h>
#include <stdint.h>

/**
 * @brief Init SDL window
 * 
 * @param w width
 * @param h height
 * @return int 
 */
int xwin_init(uint16_t w, uint16_t h);

/**
 * @brief Close SDL window
 * 
 */
void xwin_close();

/**
 * @brief Redraw SDL window with img data in format rgb888 stacked together
 * 
 * @param w width of img
 * @param h height of img
 * @param img data with rgb888 stacked together
 */
void xwin_redraw(uint16_t w, uint16_t h, uint8_t *img);

/**
 * @brief Poll event to show OS window is responsive
 * 
 * @param event 
 * @return true event was polled
 * @return false no event found
 */
bool xwin_poll_event(SDL_Event *event);

/**
 * @brief Save image of the window to file
 * 
 * @param output_name where to save the image
 */
void xwin_save_image(char *output_name);

#endif
