#include "file_open.h"
#include "file_access.h"
#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "nonblocking_io.h"
#include "serialize_lock.h"

static opened_file_state_t opened_file_create() {
  opened_file_state_t state = {
    .error = FILOPER_SUCCESS,
    .ended_with_error = false,
    .executed = false,
    .type = 0,
  };

  return state;
}

static void file_prepare_before_open() {
  serialize_unlock();
}

static void file_prepare_after_close() {
  struct termios old;
  file_set_nonblocking(STDIN_FILENO, &old);

  if (serialize_lock(1) <= 0) {
      serialize_lock(0);
  }
}

static opened_file_state_t file_execute(executing_file_t *executing, opened_type_t type) {
  executing_file_wait(executing);
  file_prepare_after_close();

  opened_file_state_t opened = opened_file_create();
  opened.type = type;
  opened.executed = true;
  opened.executing_file = *executing;
  opened.ended_with_error = executing->output_signal != 0;

  return opened;
}

static opened_file_state_t file_open_mime(file_t *file, exec_options_t *options,
                           fileaccess_state_t state, char *base_mime) {
  opened_file_state_t opened = opened_file_create();

  if (options == NULL) {
    return opened;
  }

  char mime[256];
  if (base_mime != NULL) {
    strcpy(mime, base_mime);
  } else {
    opened.error = fileaccess_file_get_mimetype(state, file, mime);

    if (opened.error != FILOPER_SUCCESS) {
      return opened;
    }
  }

  char *program = exec_options_get_program(options, mime);
  if (program == NULL) {
    return opened;
  }

  char local_path[PATH_MAX];
  opened.error =
      fileaccess_file_get_local_path(state, file, local_path);

  if (opened.error != FILOPER_SUCCESS) {
    return opened;
  }

  file_prepare_before_open();
  executing_file_error_t executing_or_error =
      executing_file_execute(program, local_path);

  if (executing_or_error.error != FILOPER_SUCCESS) {
    opened.error = executing_or_error.error;
    return opened;
  }

  return file_execute(&executing_or_error.file, base_mime != NULL ? OPENED_TEXT : OPENED_MIME);
}

static opened_file_state_t file_open_text(file_t *file, exec_options_t *options,
                           fileaccess_state_t state) {
  return file_open_mime(file, options, state, "text");
}

static opened_file_state_t file_open_executable(file_t *file, exec_options_t *options,
                                 fileaccess_state_t state) {
  opened_file_state_t opened = opened_file_create();

  if (file->permissions & S_IEXEC) {
    // executable
    file_prepare_before_open();
    executing_file_or_error_t executing_or_error =
        fileaccess_file_execute(state, file, NULL);
    if (executing_or_error.error) {
      opened.error = executing_or_error.error;
      return opened;
    }

    return file_execute(&executing_or_error.payload.file, OPENED_EXEC);
  }

  return opened;
}

opened_file_state_t file_open(file_t *file, exec_options_t *options, fileaccess_state_t state) {
  opened_file_state_t opened;

  // 1. try mime
  opened = file_open_mime(file, options, state, NULL);

  if (opened.executed || opened.error != FILOPER_SUCCESS) {
    return opened;
  }
  // 2. is executable? execute it
  opened = file_open_executable(file, options, state);

  if (opened.executed || opened.error != FILOPER_SUCCESS) {
    return opened;
  }
  // 3. text mime
  return file_open_text(file, options, state);
}
