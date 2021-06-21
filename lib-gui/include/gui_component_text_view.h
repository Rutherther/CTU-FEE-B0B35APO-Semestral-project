#ifndef __GUI_COMPONENT_TEXT_VIEW_H__
#define __GUI_COMPONENT_TEXT_VIEW_H__

#include "display_utils.h"
#include "gui.h"

typedef struct {
  char *line;
  uint16_t length;
} line_t;

typedef struct {
  line_t *lines;
  uint16_t lines_count;
  char *text;
  font_t *font;
  display_pixel_t color;
} multiline_text_t;

/**
 * @brief Create multiline text for state of text view
 * 
 * @param font 
 * @param color 
 * @param text 
 * @return multiline_text_t* 
 */
multiline_text_t *gui_multiline_text_create(font_t *font, display_pixel_t color,
                                            char *text);

/**
 * @brief Create text view component
 * 
 * @param gui 
 * @param text multiline text state
 * @param x base x coordinate
 * @param y base y coordinate
 * @return component_t 
 */
component_t gui_text_view_create(gui_t *gui, multiline_text_t *text, int16_t x,
                                 int16_t y);

/**
 * @brief Get number of lines scrolled from text view
 * 
 * @param component 
 * @return uint16_t 
 */
uint16_t gui_text_view_get_lines_scrolled(component_t *component);

/**
 * @brief Scroll text view by x, y
 * 
 * @param text_view 
 * @param x x coordinate to scroll by
 * @param y y coordinate to scroll by
 */
void gui_text_view_scroll(component_t *text_view, int32_t x, int32_t y);

/**
 * @brief Reset scroll to 0, 0
 * 
 * @param text_view 
 */
void gui_text_view_reset_scroll(component_t *text_view);

/**
 * @brief Full scroll to the end of file
 * 
 * @param text_view 
 */
void gui_text_view_full_scroll(component_t *text_view);

/**
 * @brief Render function of gui text view component
 * 
 * @param container 
 * @param component 
 * @param gui 
 */
void gui_text_view_render(container_t *container, component_t *component,
                          gui_t *gui);

/**
 * @brief Update function of gui text view component
 * 
 * @param container 
 * @param component 
 * @param gui 
 */
void gui_text_view_update(container_t *container, component_t *component,
                          gui_t *gui);

#endif // __GUI_COMPONENT_TEXT_VIEW_H__
