#ifndef __IMAGE_VIEWER_HANDLERS_H__
#define __IMAGE_VIEWER_HANDLERS_H__

#include "input.h"
#include "image_viewer_def.h"

void image_viewer_register_commands(image_viewer_t *viewer,
                                    commands_t *commands);

void image_viewer_display_image(image_viewer_t *viewer);
#endif // __IMAGE_VIEWER_HANDLERS_H__
