#include "gui.h"

component_t gui_line_create(const display_pixel_t *color, int16_t x, int16_t y, int16_t w, int16_t h);

void gui_line_render(container_t *container, component_t *component,
                          gui_t *gui);
void gui_line_update(container_t *container, component_t *component,
                     gui_t *gui);
