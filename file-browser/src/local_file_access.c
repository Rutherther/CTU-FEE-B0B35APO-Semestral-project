#include "local_file_access.h"
#include "file_access.h"
#include "local_file_utils.h"
#include "path.h"
#include <errno.h>
#include <ftw.h>
#include <linux/limits.h>
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

static uint64_t directory_get_needed_bytes(char *name, uint32_t *dirs_count, DIR *dirptr) {
  uint64_t size = sizeof(directory_t) + strlen(name) + 1;

  struct dirent *dir;
  errno = 0;
  *dirs_count = 0;
  while ((dir = readdir(dirptr)) != NULL) {
    size += sizeof(file_t);
    size += strlen(dir->d_name) + 1;
    (*dirs_count)++;
  }

  rewinddir(dirptr);
  return size;
}


static int compare_files(const void *a, const void *b) {
  const file_t *file_a = (const file_t*)a;
  const file_t *file_b = (const file_t *)b;

  return strcmp(file_a->name, file_b->name);
}
directory_or_error_t local_fileaccess_directory_list(fileaccess_state_t state,
                                                     char *path) {
  directory_or_error_t ret;
  char full_path[PATH_MAX];
  path_join((char *)state.payload.local.path, path, full_path);

  DIR *dirptr = opendir(full_path);
  if (dirptr == NULL) {
    ret.error = true;
    ret.payload.error = file_operation_error_from_errno(errno);
    return ret;
  }

  char show_path[PATH_MAX];
  realpath(path, show_path);

  uint32_t files_count = 0;
  uint64_t size = directory_get_needed_bytes(show_path, &files_count, dirptr);

  directory_t *directory = malloc(size);
  void *malloced = directory;

  if (directory == NULL) {
    ret.error = true;
    ret.payload.error = FILOPER_UNKNOWN;
    return ret;
  }

  directory_list(state, malloced, show_path, files_count, dirptr, directory);

  if (errno != 0) {
    free(malloced);
    ret.error = true;
    ret.payload.error = file_operation_error_from_errno(errno);
  } else {
    qsort(directory->files, directory->files_count, sizeof(file_t), compare_files);

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
  } else {
    strcpy(mime, data);
  }

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

file_operation_error_t
local_fileaccess_file_get_local_path(fileaccess_state_t state, file_t *file,
                                     char *out) {
  file_get_full_path(state, file->directory, file, out);
  return FILOPER_SUCCESS;
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
    .get_file_local_path = local_fileaccess_file_get_local_path
};
