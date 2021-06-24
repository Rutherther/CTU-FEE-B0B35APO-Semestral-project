#ifndef __EXTERN_FILE_ACCESS_H__
#define __EXTERN_FILE_ACCESS_H__

#include "file_access.h"

fileaccess_state_t extern_fileaccess_init_state(void *data);
bool extern_fileaccess_deinit_state(fileaccess_state_t state);

directory_or_error_t extern_fileaccess_directory_list(fileaccess_state_t state,
                                                     char *path);
directory_or_error_t extern_fileaccess_root_list(fileaccess_state_t state);

directory_or_error_t extern_fileaccess_directory_create(fileaccess_state_t state,
                                                       char *path);
file_operation_error_t
extern_fileaccess_directory_delete(fileaccess_state_t state, char *path);

file_operation_error_t
extern_fileaccess_directory_close(fileaccess_state_t state,
                                 directory_t *directory);

file_operation_error_t
extern_fileaccess_file_get_mime_type(fileaccess_state_t state, file_t *file,
                                    char *mime);
executing_file_or_error_t
extern_fileaccess_file_execute(fileaccess_state_t state, file_t *file,
                              char *args);
file_operation_error_t extern_fileaccess_file_delete(fileaccess_state_t state,
                                                    char *path);

#endif // __EXTERN_FILE_ACCESS_H__
