#include "file_open.h"
#include "file_access.h"
#include <linux/limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include "nonblocking_io.h"
#include "serialize_lock.h"

static void file_prepare_before_open() {
  serialize_unlock();
}

static void file_prepare_after_open() {
  struct termios old;
  file_set_nonblocking(STDIN_FILENO, &old);

  if (serialize_lock(1) <= 0) {
      serialize_lock(0);
  }
}

file_operation_error_t file_open(file_t *file, exec_options_t *options, fileaccess_state_t state) {
  executing_file_t executing;
  if (file->permissions & S_IXUSR) {
    // executable
    file_prepare_before_open();
    executing_file_or_error_t executing_or_error = fileaccess_file_execute(state, file, "");
    if (executing_or_error.error) {
      file_prepare_after_open();
      return executing_or_error.payload.error;
    }
    
    executing = executing_or_error.payload.file;
  } else if (options != NULL) {
    char mime[256];
    fileaccess_file_get_mimetype(state, file, mime);

    char *program = exec_options_get_program(options, mime);

    if (program == NULL) {
      program = exec_options_get_program(options, "text");
    }

    if (program == NULL) {
      return FILOPER_SUCCESS;
    }

    char local_path[PATH_MAX];
    file_operation_error_t error = fileaccess_file_get_local_path(state, file, local_path);

    if (error != FILOPER_SUCCESS) {
      return error;
    }

    file_prepare_before_open();
    executing_file_error_t executing_or_error = executing_file_execute(program, local_path);

    if (executing_or_error.error != FILOPER_SUCCESS) {
      file_prepare_after_open();
      return executing_or_error.error;
    }

    executing = executing_or_error.file;
  } else {
    return FILOPER_UNKNOWN;
  }

  executing_file_wait(&executing);
  file_prepare_after_open();
  // TODO: figure out return data?
  return FILOPER_SUCCESS;
}
