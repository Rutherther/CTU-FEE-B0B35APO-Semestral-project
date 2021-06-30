#ifndef __FILE_BROWSER_UTILS_H__
#define __FILE_BROWSER_UTILS_H__

#include "file_open.h"
#include "gui.h"
#include "font.h"

void file_browser_handle_opened_file(opened_file_state_t opened, gui_t *gui,
                                     font_t *font);

#endif // __FILE_BROWSER_UTILS_H__
