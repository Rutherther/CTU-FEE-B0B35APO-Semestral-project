#ifndef __FILE_OPEN_H__
#define __FILE_OPEN_H__

#include "file_access.h"
#include "options.h"
#include <stdbool.h>

typedef enum {
  OPENED_NONE,
  OPENED_MIME,
  OPENED_EXEC,
  OPENED_TEXT,
} opened_type_t;

typedef struct {
  opened_type_t type;

  bool executed;
  bool ended_with_error;

  file_operation_error_t error;

  executing_file_t executing_file;
} opened_file_state_t;

opened_file_state_t file_open(file_t *file, exec_options_t *options,
                              fileaccess_state_t state);

opened_file_state_t file_open_text(file_t *file, exec_options_t *options,
                                   fileaccess_state_t state);

opened_file_state_t file_open_mime_raw(char *file, exec_options_t *options,
                                       char *mime);

#endif // __FILE_OPEN_H__
