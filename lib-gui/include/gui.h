#ifndef __GUI_H__
#define __GUI_H__

#include "display_utils.h"
#include "font.h"
#include "input.h"
#include "logger.h"
#include "mzapo_pheripherals.h"

typedef struct renderer_t renderer_t;
typedef struct component_t component_t;
typedef struct gui_t gui_t;
typedef struct container_t container_t;

typedef void (*render_function)(container_t *container, component_t *component,
                                gui_t *gui);
typedef void (*update_function)(container_t *container, component_t *component,
                                gui_t *gui);

struct component_t {
  int16_t x;
  int16_t y;

  uint16_t width;
  uint16_t height;

  render_function render;
  update_function update;

  bool focusable;
  bool focused;

  void *state;
};

typedef enum {
  CONT_TABLE,
  CONT_GROUP,
  CONT_ONE,
} container_type_t;

typedef struct {
  // items
  // scroll
} list_container_t;

typedef struct {
  component_t *components;
  uint16_t size;
  uint16_t count;
} group_container_t;

typedef struct {
  component_t component;
  bool set;
} one_container_t;

typedef union {
  list_container_t list;
  group_container_t group;
  one_container_t one;
} container_inner_t;

struct container_t {
  container_type_t type;
  container_inner_t inner;

  bool focusable;
  bool focused;

  int16_t x;
  int16_t y;

  uint16_t width;
  uint16_t height;
};

typedef struct {
  container_t *containers;
  uint16_t containers_size;
  uint16_t containers_count;
} window_t;

struct gui_t {
  window_t *active_window;
  commands_t *commands;
  renderer_t *renderer;
  mzapo_pheripherals_t *pheripherals;
  logger_t *logger;

  size2d_t size;
};

/**
 * @brief Create gui state
 *
 * @param logger
 * @param commands
 * @param renderer
 * @param pheripherals
 * @return gui_t
 */
gui_t gui_create(logger_t *logger, commands_t *commands, renderer_t *renderer,
                 mzapo_pheripherals_t *pheripherals);

/**
 * @brief Render all elements in gui
 *
 * @param gui
 */
void gui_render(gui_t *gui);

/**
 * @brief Update all elements in gui
 *
 * @param gui
 */
void gui_update(gui_t *gui);

/**
 * @brief Set gui active window
 *
 * @param gui
 * @param window
 */
void gui_set_active_window(gui_t *gui, window_t *window);

// gui_window.c
/**
 * @brief Create new gui window
 *
 * @param containers array of containers of static size
 * @param size maximal number of the containers
 * @return window_t
 */
window_t gui_window_create(container_t *containers, uint16_t size);

/**
 * @brief Add container to window
 *
 * @param window
 * @param container
 * @return container_t*
 */
container_t *gui_window_add_container(window_t *window, container_t container);

// gui_container.c
/**
 * @brief Render all elements inside container
 *
 * @param gui
 * @param container
 */
void gui_container_render(gui_t *gui, container_t *container);

/**
 * @brief Update all elements inside container
 *
 * @param gui
 * @param container
 */
void gui_container_update(gui_t *gui, container_t *container);

// gui_component.c
/**
 * @brief Create gui component
 *
 * @param x begin x coord
 * @param y begin y coord
 * @param w width
 * @param h height
 * @param render
 * @param update
 * @return component_t
 */
component_t gui_component_create(int16_t x, int16_t y, uint16_t w, uint16_t h,
                                 render_function render,
                                 update_function update);

/**
 * @brief Check whether component is within screen so it may be rendered
 *
 * @param gui
 * @param container
 * @param component
 * @return true
 * @return false
 */
bool gui_is_component_visible(gui_t *gui, container_t *container,
                              component_t *component);

/**
 * @brief Render gui component
 *
 * @param gui
 * @param container
 * @param component
 */
void gui_component_render(gui_t *gui, container_t *container,
                          component_t *component);

/**
 * @brief Update gui component
 *
 * @param gui
 * @param container
 * @param component
 */
void gui_component_update(gui_t *gui, container_t *container,
                          component_t *component);

/**
 * @brief Get absolute position of component
 *
 * @param container
 * @param component
 * @return coords_t
 */
coords_t gui_component_get_absolute_position(container_t *container,
                                             component_t *component);

/**
 * @brief Get position on screen
 *
 * @param container
 * @param component
 * @return coords_t
 */
coords_t gui_component_get_screen_position(container_t *container,
                                           component_t *component);

// gui_one_container.c
/**
 * @brief Create ONE container that holds only one component
 *
 * @param x
 * @param y
 * @return container_t
 */
container_t gui_one_container_create(int16_t x, int16_t y);

/**
 * @brief Set ONE container component
 *
 * @param container
 * @param component
 * @return component_t* set component
 */
component_t *gui_one_container_set_component(container_t *container,
                                             component_t component);

/**
 * @brief Get ONE container component if it is set
 *
 * @param container
 * @return component_t*
 */
component_t *gui_one_container_get_component(container_t *container);

/**
 * @brief Render ONE container
 *
 * @param gui
 * @param container
 */
void gui_one_container_render(gui_t *gui, container_t *container);

/**
 * @brief Update ONE container
 *
 * @param gui
 * @param container
 */
void gui_one_container_update(gui_t *gui, container_t *container);

// gui_group_container.c
/**
 * @brief Create GROUP container holding n components
 *
 * @param x
 * @param y
 * @param components array of components of fixed size
 * @param components_size size of components
 * @return container_t
 */
container_t gui_group_container_create(int16_t x, int16_t y,
                                       component_t *components,
                                       uint16_t components_size);

/**
 * @brief Add component to GROUP container
 *
 * @param container
 * @param component
 * @return component_t*
 * @return NULL if container is full
 */
component_t *gui_group_container_add_component(container_t *container,
                                               component_t component);

void gui_group_container_render(gui_t *gui, container_t *container);
void gui_group_container_update(gui_t *gui, container_t *container);

// handle commands

#endif // __GUI_H__
