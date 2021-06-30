#ifndef __TEMP_FILE_ACCESS_H__
#define __TEMP_FILE_ACCESS_H__

#include "file_access.h"

fileaccess_state_t temp_fileaccess_init_state(void *data);
bool temp_fileaccess_deinit_state(fileaccess_state_t state);

#endif // __TEMP_FILE_ACCESS_H__
