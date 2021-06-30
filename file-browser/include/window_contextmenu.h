#ifndef __WINDOW_CONTEXTMENU_H__
#define __WINDOW_CONTEXTMENU_H__

#include "file_access.h"
#include "font.h"
#include "gui.h"
#include <stdbool.h>

bool window_contextmenu_open(gui_t *gui, font_t *font, fileaccess_state_t state,
                             file_t *file);

#endif // __WINDOW_CONTEXTMENU_H__
