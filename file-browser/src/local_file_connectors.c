#include "local_file_connectors.h"
#include "file_access.h"
#include "local_file_utils.h"
#include "path.h"
#include <stdio.h>
#include <errno.h>

#define BUFFER_SIZE 1024

file_operation_error_t local_connector_file_copy(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination) {
  char src_path[file_get_full_path_memory_size(first_state, file->directory,
                                               file)];
  file_get_full_path(first_state, file->directory, file, src_path);

  char dst_path[path_join_memory_size(second_state.payload.local.path, destination)];
  path_join(second_state.payload.local.path, destination, dst_path);

  // TODO: copy folders

  FILE *src_file = fopen(src_path, "r");
  if (src_file == NULL) {
    return file_operation_error_from_errno(errno);
  }
  FILE *dst_file = fopen(dst_path, "w");
  if (dst_file == NULL) {
    fclose(src_file);
    return file_operation_error_from_errno(errno);
  }

  while (!feof(src_file) && !ferror(src_file) && !ferror(dst_file)) {
    char buffer[BUFFER_SIZE];
    unsigned long in = fread(buffer, sizeof(char), BUFFER_SIZE, src_file);
    if (in == 0) {
      continue;
    }
    fwrite(buffer, sizeof(char), in, dst_file);
  }

  file_operation_error_t error = FILOPER_SUCCESS;
  if (!feof(src_file) && ferror(src_file)) {
    error = file_operation_error_from_errno(ferror(src_file));
  } else if (ferror(dst_file)) {
    error = file_operation_error_from_errno(ferror(dst_file));
  }

  fclose(src_file);
  fclose(dst_file);
  return error;
}

file_operation_error_t local_connector_file_move(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination) {
  file_operation_error_t error = FILOPER_SUCCESS;
  char src_path[file_get_full_path_memory_size(first_state, file->directory,
                                               file)];
  char dst_path[path_join_memory_size(second_state.payload.local.path, destination)];
  path_join(second_state.payload.local.path, destination, dst_path);

  int status = rename(src_path, dst_path);
  if (status == 0) {
    return error;
  }

  if (errno == EXDEV) {
    // if rename does not work
    file_get_full_path(first_state, file->directory, file, src_path);

    error = connector->copy(connector, first_state, second_state, file,
                            destination);
    if (error == FILOPER_SUCCESS) {
      if (file->type == FT_FOLDER) {
        error = directory_delete(src_path);
      } else {
        error = file_delete(src_path);
      }
    }
  } else {
    error = file_operation_error_from_errno(errno);
  }

  return error;
}

fileaccess_connector_t fileaccess_connectors[(FA_COUNT - 1) * FA_COUNT] = {
    {.first = &local_file_access,
     .second = &temp_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},

    {.first = &temp_file_access,
     .second = &local_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},

    {.first = &local_file_access,
     .second = &extern_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},
    {.first = &extern_file_access,
     .second = &local_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},

    {.first = &extern_file_access,
     .second = &temp_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},
    {.first = &temp_file_access,
     .second = &extern_file_access,
     .move = local_connector_file_move,
     .copy = local_connector_file_copy},
};
