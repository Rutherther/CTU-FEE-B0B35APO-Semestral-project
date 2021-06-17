#include "input.h"
#include "nonblocking_io.h"
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#define ROTATION_DELTA 0
#define ENCODERS_MAX 255
#define DEBOUNCE_TIME 0.2 // seconds

void commands_update_rotation_encoders(rotation_encoders_t *commands);

void commands_rotation_encoders_init(commands_t *commands) {
  for (int i = 0; i < ROTATION_ENCODERS_COUNT; i++) {
    rotation_encoder_state_t state = {
      .absolute = 0,
      .delta = 0,
    };
    commands->encoders.encoders_state[i] = state;
  }

  commands_update_rotation_encoders(&commands->encoders);
}

commands_t commands_create(command_t *array, uint8_t size, void *reg_knobs_base) {
  commands_t commands = {
    .size = size,
    .count = 0,
    .commands = array,
    .encoders = {
      .base_address = reg_knobs_base
    }
  };

  commands_rotation_encoders_init(&commands);

  return commands;
}

bool commands_register(commands_t * commands, input_type_t type, char filter,
                        command_fun fun, void *state) {
  if (commands->count >= commands->size) {
    return false;
  }

  command_t command = {
      .state = state,
      .cmd = fun,
      .input = {.filter = filter, .type = type},
  };

  commands->commands[commands->count++] = command;
  return true;
}

bool commands_unregister(commands_t * commands, command_t * command) {
  int found = -1;
  for (int i = 0; i < commands->count; i++) {
    if (&commands->commands[i] == command) {
      found = i;
    }
  }

  if (found == -1) {
    return false;
  }

  for (int i = found; i < commands->count - 1; i++) {
    commands->commands[i] = commands->commands[i + 1];
  }
  commands->count--;

  return true;
}

int16_t commands_execute(commands_t * commands, input_type_t type, char filter,
                        int amount) {
  int16_t commands_executed = 0;
  for (int i = 0; i < commands->count; i++) {
    command_t command = commands->commands[i];

    if (command.input.filter == filter && command.input.type == type) {
      command.cmd(command.state, amount);
      commands_executed++;
    }
  }

  return commands_executed;
}

void commands_update_rotation_encoders(rotation_encoders_t *encoders) {
  if (encoders->base_address == NULL) {
    for (int i = 0; i < ROTATION_ENCODERS_COUNT; i++) {
      encoders->encoders_state[i].absolute = 0;
      encoders->encoders_state[i].delta = 0;
      encoders->encoders_state[i].button = false;
      encoders->encoders_state[i].button_prev = false;
    }
    return;
  }

  uint8_t btns = *(volatile uint8_t*)(encoders->base_address + ROTATION_ENCODERS_COUNT);

  for (int i = 0; i < ROTATION_ENCODERS_COUNT; i++) {
    rotation_encoder_state_t state = encoders->encoders_state[i];
    uint8_t rotation = *(volatile uint8_t*)(encoders->base_address + i);
    bool btn = (btns >> (7 - i)) & 1;

    uint8_t diff = rotation - state.absolute;

    state.delta = (int8_t)diff;
    state.absolute = rotation;

    time_t now = time(NULL);
    if (btn && !state.button) {
      state.button = true;
      state.button_prev = false;
      state.pressed_time = time(NULL);

    } else if (!btn && state.button &&
               difftime(now, state.pressed_time) > DEBOUNCE_TIME) {
      state.button = false;
      state.button_prev = true;
    }

    encoders->encoders_state[i] = state;
  }
}

int16_t commands_check_input(commands_t * commands) {
  int16_t commands_executed = 0;

  uint8_t data;
  int read;
  while ((read = file_read_nonblocking(STDIN_FILENO, 1, &data)) > 0) {
    commands_executed += commands_execute(commands, IN_KEYBOARD, data, 5);
  }

  if (read < 0) {
    return -1;
  }

  commands_update_rotation_encoders(&commands->encoders);
  rotation_encoders_t encoders = commands->encoders;
  for (int i = 0; i < commands->size; i++) {
    command_t command = commands->commands[i];
    input_t input = command.input;
    if (input.type != IN_ENCODER_ROTATE && input.type != IN_ENCODER_CLICK) {
      continue;
    }

    rotation_encoder_state_t state = encoders.encoders_state[(uint8_t)command.input.filter];
    if (command.input.filter > ROTATION_ENCODERS_COUNT) {
      return -1;
    }

    bool button_changed = state.button_prev != state.button;
    bool rotation_changed = state.delta != 0;

    if (input.type == IN_ENCODER_ROTATE && rotation_changed) {
      command.cmd(command.state, state.delta);
    } else if (input.type == IN_ENCODER_CLICK && button_changed) {
      command.cmd(command.state, state.button);
    }
  }

  return commands_executed;
}
