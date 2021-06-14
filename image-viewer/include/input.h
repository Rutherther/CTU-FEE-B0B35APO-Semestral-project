#include <stdint.h>
#include <stdbool.h>
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
  void* base_address;
  rotation_encoder_state_t encoders_state[ROTATION_ENCODERS_COUNT];
} rotation_encoders_t;

typedef struct {
  command_fun cmd;
  input_t input;
  void *state;
} command_t;

typedef struct {
  command_t *commands;
  uint8_t count;
  uint8_t size;

  rotation_encoders_t encoders;
} commands_t;

commands_t commands_create(command_t *array, uint8_t size,
                           void *reg_knobs_base);

bool commands_register(commands_t *commands, input_type_t type,
                       char filter, command_fun fun, void *state);

bool commands_unregister(commands_t *commands, command_t *command);

short commands_check_input(commands_t *commands);
