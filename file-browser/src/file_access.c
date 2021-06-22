#include "file_access.h"
#include <string.h>

fileaccess_state_t fileaccess_init(const fileaccess_t *fileaccess, void *data) {
  return fileaccess->init(data);
}

bool fileaccess_deinit(fileaccess_state_t state) {
  return state.fileaccess->deinit(state);
}

directory_or_error_t fileaccess_directory_list(fileaccess_state_t state,
                                               char *path) {
  return state.fileaccess->list_directory(state, path);
}

directory_or_error_t fileaccess_root_list(fileaccess_state_t state) {
  return state.fileaccess->list_root(state);
}

directory_or_error_t fileaccess_directory_create(fileaccess_state_t state,
                                                 char *path) {
  return state.fileaccess->create_directory(state, path);
}

file_operation_error_t fileaccess_directory_close(fileaccess_state_t state,
                                                  directory_t *directory) {
  return state.fileaccess->close_directory(state, directory);
}

file_operation_error_t fileaccess_file_get_mimetype(fileaccess_state_t state,
                                                    file_t *file,
                                                    /*out*/ char *mime) {
  return state.fileaccess->get_mime_type(state, file, mime);
}

pid_or_error_t fileaccess_file_execute(fileaccess_state_t state, file_t *file,
                                       char *args) {
  return state.fileaccess->execute_file(state, file, args);
}
