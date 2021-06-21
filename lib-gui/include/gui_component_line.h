#include "gui.h"

/**
 * @brief Create gui line component
 *
 * @param color
 * @param x base x coordinate
 * @param y base y coordinate
 * @param w width
 * @param h height
 * @return component_t
 */
component_t gui_line_create(const display_pixel_t *color, int16_t x, int16_t y,
                            int16_t w, int16_t h);

/**
 * @brief Render function of gui line component
 *
 * @param container
 * @param component
 * @param gui
 */
void gui_line_render(container_t *container, component_t *component,
                     gui_t *gui);

/**
 * @brief Update function of gui line component
 *
 * @param container
 * @param component
 * @param gui
 */
void gui_line_update(container_t *container, component_t *component,
                     gui_t *gui);
