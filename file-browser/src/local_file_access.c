#include "local_file_access.h"
#include "file_access.h"
#include "local_file_utils.h"
#include "path.h"
#include <errno.h>
#include <ftw.h>
#include <magic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define ROOT "/"

fileaccess_state_t local_fileaccess_init_state(void *data) {
  fileaccess_state_t state = {.payload.local.path = ROOT, .fileaccess = &local_file_access};
  return state;
}

bool local_fileaccess_deinit_state(fileaccess_state_t data) {
  // nothing needs to be done
  return true;
}

static file_operation_error_t file_get_information(void **malloced,
                                                   uint64_t *offset, uint64_t *bytes_malloced,
                                                   fileaccess_state_t state,
                                                   file_t file) {
  size_t name_len = strlen(file.name);
  bytes_malloced += sizeof(file_t) + name_len + 1;
  void *new = realloc(*malloced, *bytes_malloced);
  if (new == NULL) {
    free(*malloced);
    return FILOPER_UNKNOWN;
  }
  *malloced = new;

  char full_path[file_get_full_path_memory_size(state, file.directory, &file)];
  file_get_full_path(state, file.directory, &file, full_path);

  // load info
  struct stat stats;
  int status = stat(full_path, &stats);

  if (status == -1) {
    free(new);
    return file_operation_error_from_errno(errno);
  }

  file.size = stats.st_size;
  file.gid = stats.st_gid;
  file.uid = stats.st_uid;
  file.permissions = stats.st_mode;

  file_t *stored = new + *offset;
  *stored = file;
  *offset += sizeof(file_t);

  strcpy(new + *offset, file.name);
  stored->name = new + *offset;
  *offset += name_len + 1;

  return FILOPER_SUCCESS;
}

directory_or_error_t local_fileaccess_directory_list(fileaccess_state_t state,
                                                     char *path) {
  directory_or_error_t ret;
  char full_path[path_join_memory_size(state.payload.local.path, path)];
  path_join((char *)state.payload.local.path, path, full_path);

  uint64_t malloc_offset = sizeof(directory_t) + strlen(path) + 1;
  uint64_t bytes_malloced = sizeof(directory_t) + strlen(path) + 1;
  directory_t *directory = malloc(malloc_offset);
  void *malloced = directory;

  if (directory == NULL) {
    ret.error = true;
    ret.payload.error = FILOPER_UNKNOWN;
    return ret;
  }

  directory->path = malloced + sizeof(directory_t);
  strcpy(directory->path, path);


  DIR *dirptr = opendir(full_path);
  if (dirptr == NULL) {
    ret.error = true;
    ret.payload.error = file_operation_error_from_errno(errno);
    free(malloced);
    return ret;
  }

  struct dirent * dir;
  errno = 0;
  while ((dir = readdir(dirptr)) != NULL) {
    file_t file;
    file.directory = directory;
    file.name = dir->d_name;

    switch(dir->d_type) {
    case DT_DIR:
      file.type = FT_FOLDER;
      break;
    case DT_REG:
      file.type = FT_FILE;
      break;
    case DT_UNKNOWN:
      file.type = FT_UNKNOWN;
      break;
    default:
      file.type = FT_OTHER;
      break;
    }

    ret.payload.error = file_get_information(&malloced, &malloc_offset,
                                             &bytes_malloced, state, file);
    if (ret.payload.error != FILOPER_SUCCESS) {
      ret.error = true;
      free(malloced);
      return ret;
    }

    directory->files_count++;
  }

  if (errno != 0) {
    ret.error = true;
    ret.payload.error = file_operation_error_from_errno(errno);
  } else {
    ret.error = false;
    ret.payload.directory = directory;
  }

  return ret;
}

directory_or_error_t local_fileaccess_root_list(fileaccess_state_t state) {
  return local_fileaccess_directory_list(state, (char *)state.payload.local.path);
}

directory_or_error_t local_fileaccess_directory_create(fileaccess_state_t state,
                                                       char *path) {
  directory_or_error_t ret;
  char full_path[path_join_memory_size(state.payload.local.path, path)];
  path_join((char *)state.payload.local.path, path, full_path);

  int status = mkdir(full_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  if (status == -1) {
    ret.error = true;
    ret.payload.error = file_operation_error_from_errno(errno);
  } else {
    ret = local_fileaccess_directory_list(state, full_path);
  }

  return ret;
}

file_operation_error_t
local_fileaccess_directory_delete(fileaccess_state_t state, char *path) {
  file_operation_error_t error = FILOPER_SUCCESS;
  char full_path[path_join_memory_size(state.payload.local.path, path)];
  path_join((char *)state.payload.local.path, path, full_path);

  int status = directory_delete(full_path);
  if (status != 0) {
    error = status;
  }

  return error;
}

file_operation_error_t
local_fileaccess_directory_close(fileaccess_state_t state,
                                 directory_t *directory) {
  free(directory);
  return FILOPER_SUCCESS;
}

file_operation_error_t
local_fileaccess_file_get_mime_type(fileaccess_state_t state, file_t *file,
                                    char *mime) {
  file_operation_error_t error = FILOPER_SUCCESS;

  magic_t magic = magic_open(MAGIC_MIME_TYPE);
  magic_load(magic, NULL);

  char full_path[file_get_full_path_memory_size(state, file->directory, file)];
  file_get_full_path(state, file->directory, file, full_path);

  const char *data = magic_file(magic, full_path);
  if (data == NULL) {
    error = file_operation_error_from_errno(errno);
  }

  uint16_t i = 0;
  while (*data != '\0') {
    mime[i++] = *data;
  }
  mime[i] = '\0';

  magic_close(magic);
  return error;
}

executing_file_or_error_t local_fileaccess_file_execute(fileaccess_state_t state,
                                             file_t *file, char *args) {
  executing_file_or_error_t ret;
  char full_path[file_get_full_path_memory_size(state, file->directory, file)];
  file_get_full_path(state, file->directory, file, full_path);
  // TODO: check permissions

  executing_file_error_t efile = executing_file_execute(full_path, args);
  if (efile.error == true) {
    ret.error = true;
    ret.payload.error = FILOPER_UNKNOWN;
  } else {
    ret.error = false;
    ret.payload.file = efile.file;
  }

  return ret;
}

file_operation_error_t local_fileaccess_file_delete(fileaccess_state_t state,
                                                    char *path) {
  file_operation_error_t error = FILOPER_SUCCESS;
  char full_path[path_join_memory_size(state.payload.local.path, path)];
  path_join((char *)state.payload.local.path, path, full_path);

  int status = remove(path);

  if (status == -1) {
    error = file_operation_error_from_errno(errno);
  }

  return error;
}

const fileaccess_t local_file_access = {
    .type = FA_LOCAL,
    .delete_file = local_fileaccess_file_delete,
    .delete_directory = local_fileaccess_directory_delete,
    .close_directory = local_fileaccess_directory_close,
    .create_directory = local_fileaccess_directory_create,
    .list_directory = local_fileaccess_directory_list,
    .list_root = local_fileaccess_root_list,
    .init = local_fileaccess_init_state,
    .deinit = local_fileaccess_deinit_state,
    .get_mime_type = local_fileaccess_file_get_mime_type,
    .execute_file = local_fileaccess_file_execute,
};
