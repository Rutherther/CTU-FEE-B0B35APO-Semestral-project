#ifndef __FILE_OPEN_H__
#define __FILE_OPEN_H__

#include <stdbool.h>
#include "file_access.h"
#include "options.h"

file_operation_error_t file_open(file_t *file, exec_options_t *options,
                                 fileaccess_state_t state);

#endif // __FILE_OPEN_H__
