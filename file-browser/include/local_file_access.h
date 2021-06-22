#ifndef __LOCAL_FILE_ACCESS_H__
#define __LOCAL_FILE_ACCESS_H__

#include "file_access.h"

fileaccess_state_t local_fileaccess_init_state(void *data);
bool local_fileaccess_deinit_state(fileaccess_state_t state);

directory_or_error_t local_fileaccess_directory_list(fileaccess_state_t state, char *path);
directory_or_error_t local_fileaccess_root_list(fileaccess_state_t state);

directory_or_error_t local_fileaccess_directory_create(fileaccess_state_t state, char *path);
file_operation_error_t local_fileaccess_directory_delete(fileaccess_state_t state, char *path);

file_operation_error_t local_fileaccess_directory_close(fileaccess_state_t state, directory_t *directory);

file_operation_error_t local_fileaccess_file_get_mime_type(fileaccess_state_t state, file_t *file, char *mime);
pid_or_error_t local_fileaccess_file_execute(fileaccess_state_t state, file_t *file, char *args);
file_operation_error_t local_fileaccess_file_delete(fileaccess_state_t state, char *path);

#endif // __LOCAL_FILE_ACCESS_H__
