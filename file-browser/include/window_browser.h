#ifndef __WINDOW_BROWSER_H__
#define __WINDOW_BROWSER_H__

#include "gui.h"
#include <stdbool.h>
#include "file_access.h"

bool window_browser_open_local(gui_t *gui, font_t *font);
bool window_browser_open(gui_t *gui, font_t *font, fileaccess_state_t state);

#endif // __WINDOW_BROWSER_H__
