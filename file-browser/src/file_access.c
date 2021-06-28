#include "file_access.h"
#include "logger.h"
#include <string.h>
#include <errno.h>
/*
  FILOPER_SUCCESS,
  FILOPER_PERMISSIONS,
  FILOPER_DOES_NOT_EXIST,
  FILOPER_USED,
  FILOPER_ALREADY_EXISTS,
  FILOPER_NOT_ENOUGH_SPACE,
  FILOPER_UNKNOWN,
 */

const char *file_operation_error_strings[] = {
  "Success",
  "No permissions",
  "No such file or directory",
  "File is in use",
  "File already exists",
  "Not enough space on device",
  "Unknown error",
};

void fileaccess_log_error(logger_t *logger, file_operation_error_t error) {
  logger_error(logger, __FILE__, __FUNCTION__, __LINE__,
               "File operation error: %s", file_operation_error_strings[error]);
}

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

file_operation_error_t fileaccess_directory_delete(fileaccess_state_t state,
                                                   char *path) {
  return state.fileaccess->delete_directory(state, path);
}

file_operation_error_t fileaccess_file_get_mimetype(fileaccess_state_t state,
                                                    file_t *file,
                                                    /*out*/ char *mime) {
  return state.fileaccess->get_mime_type(state, file, mime);
}

executing_file_or_error_t fileaccess_file_execute(fileaccess_state_t state, file_t *file,
                                       char *args) {
  return state.fileaccess->execute_file(state, file, args);
}

file_operation_error_t fileaccess_file_delete(fileaccess_state_t state,
                                              char *path) {
  return state.fileaccess->delete_file(state, path);
}

file_operation_error_t fileaccess_file_get_local_path(fileaccess_state_t state,
                                                      file_t *file, char *out) {
  return state.fileaccess->get_file_local_path(state, file, out);
}

file_operation_error_t file_operation_error_from_errno(int error) {
  switch (error) {
  case EACCES:
  case EROFS:
    return FILOPER_PERMISSIONS;
  case EEXIST:
  case ENOTDIR:
    return FILOPER_ALREADY_EXISTS;
  case ENOENT:
    return FILOPER_DOES_NOT_EXIST;
  case ENOSPC:
    return FILOPER_NOT_ENOUGH_SPACE;
  default:
    return FILOPER_UNKNOWN;
  }
}
