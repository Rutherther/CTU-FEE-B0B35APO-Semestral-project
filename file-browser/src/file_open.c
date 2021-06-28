#include "file_open.h"
#include "file_access.h"
#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "nonblocking_io.h"
#include "serialize_lock.h"

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

static void file_execute(executing_file_t *executing) {
  executing_file_wait(executing);
  file_prepare_after_close();
}

static bool file_open_mime(file_t *file, exec_options_t *options,
                           fileaccess_state_t state, char *base_mime,
                           file_operation_error_t *error) {
  if (options == NULL) {
    return false;
  }

  char mime[256];
  if (base_mime != NULL) {
    strcpy(mime, base_mime);
  } else {
    *error = fileaccess_file_get_mimetype(state, file, mime);

    if (*error != FILOPER_SUCCESS) {
      return true;
    }
  }

  char *program = exec_options_get_program(options, mime);
  if (program == NULL) {
    return false;
  }

  char local_path[PATH_MAX];
  *error =
      fileaccess_file_get_local_path(state, file, local_path);

  if (*error != FILOPER_SUCCESS) {
    return true;
  }

  file_prepare_before_open();
  executing_file_error_t executing_or_error =
      executing_file_execute(program, local_path);

  if (executing_or_error.error != FILOPER_SUCCESS) {
    return executing_or_error.error;
  }

  file_execute(&executing_or_error.file);

  return true;
}

static bool file_open_text(file_t *file, exec_options_t *options,
                           fileaccess_state_t state,
                           file_operation_error_t *error) {
  return file_open_mime(file, options, state, "text", error);
}

static bool file_open_executable(file_t *file, exec_options_t *options,
                                 fileaccess_state_t state,
                                 file_operation_error_t *error) {
  if (file->permissions & S_IXUSR) {
    // executable
    file_prepare_before_open();
    executing_file_or_error_t executing_or_error =
        fileaccess_file_execute(state, file, "");
    if (executing_or_error.error) {
      *error = executing_or_error.error;
      return true;
    }

    file_execute(&executing_or_error.payload.file);
    return true;
  }

  return false;
}

file_operation_error_t file_open(file_t *file, exec_options_t *options, fileaccess_state_t state) {
  file_operation_error_t error = FILOPER_SUCCESS;

  // 1. try mime
  if (file_open_mime(file, options, state, NULL, &error)) {
    return error;
  }

  if (error != FILOPER_SUCCESS) {
    return error;
  }
  // 2. is executable? execute it
  if (file_open_executable(file, options, state, &error)) {
    return error;
  }

  if (error != FILOPER_SUCCESS) {
    return error;
  }
  // 3. text mime
  file_open_text(file, options, state, &error);

  // TODO: figure out return data?
  return error;
}
