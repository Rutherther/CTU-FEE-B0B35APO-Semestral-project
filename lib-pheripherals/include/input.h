#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define ROTATION_ENCODERS_COUNT 3

typedef void (*command_fun)(void *state, int delta);

typedef enum {
  IN_KEYBOARD,
  IN_ENCODER_ROTATE,
  IN_ENCODER_CLICK,
} input_type_t;

typedef struct {
  input_type_t type;
  char filter;
} input_t;

typedef struct {
  uint8_t absolute;
  int8_t delta;

  bool button;
  bool button_prev;

  time_t pressed_time;
} rotation_encoder_state_t;

typedef struct {
  void *base_address;
  rotation_encoder_state_t encoders_state[ROTATION_ENCODERS_COUNT];
} rotation_encoders_t;

typedef struct {
  command_fun cmd;
  input_t input;
  void *state;
} command_t;

typedef struct {
  command_t *commands;
  uint16_t count;
  uint16_t size;

  rotation_encoders_t encoders;
} commands_t;

/**
 * @brief Create and initialize commands structure to use for handling input
 *
 * @param array of commands with lifetime at least as big as commands_t lifetime
 * @param size maximum number of commands in array
 * @param reg_knobs_base address of reg knobs in memory to get their data
 * @return commands_t
 */
commands_t commands_create(command_t *array, uint8_t size,
                           void *reg_knobs_base);

/**
 * @brief Add new command to commands to handle input with
 *
 * @param commands
 * @param type what type of input to look for
 * @param filter depending on the type this means either a char key or index of
 * rotation encoder
 * @param fun function that will be called when correct input is received
 * @param state state to pass to fun on command call with lifetime bigger than
 * command
 * @return true on success
 * @return false on error (full list)
 */
bool commands_register(commands_t *commands, input_type_t type, char filter,
                       command_fun fun, void *state);

/**
 * @brief Delete/Unregister command from commands so its function is not called
 * anymore
 *
 * @param commands commands to delete from
 * @param command command to delete
 * @return true command found and deleted
 * @return false command not found
 */
bool commands_unregister(commands_t *commands, command_t *command);

/**
 * @brief Check new state of input and if conditions for any of the commands are
 * met, call its function
 *
 * @param commands
 * @return short number of commands called or -1 in case of an error
 */
short commands_check_input(commands_t *commands);

uint16_t commands_save_state(commands_t *commands);

bool commands_restore_state(commands_t *commands, uint16_t state);

#endif // __INPUT_H__
