#include "local_file_utils.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "path.h"
#include <errno.h>
#include <ftw.h>
#include <string.h>
#include <stdio.h>

static int nfw_callback(const char *fpath, const struct stat *sb, int typeflag);

size_t file_get_full_path_memory_size(fileaccess_state_t state, directory_t *directory, file_t *file) {
  return strlen(file->name) + strlen(state.payload.local.path) + strlen(file->name) + 3;
}

bool file_get_full_path(fileaccess_state_t state,
                        directory_t *directory, file_t *file, char *out) {
  char base_path[path_join_memory_size(state.payload.local.path, directory->path)];
  if (!path_join((char*)state.payload.local.path, directory->path, base_path)) {
    return false;
  }

  if (!path_join(base_path, file->name, out)) {
    return false;
  }

  return true;
}

int directory_delete(const char *path) {
  int err = ftw(path, nfw_callback, 5);
  if (err != 1) {
    err = FILOPER_UNKNOWN;
  }

  if (err != 0) {
    return err;
  }

  return file_delete(path);
}

int file_delete(const char *path) {
  int err = 0;
  if (remove(path) == -1) {
    err = file_operation_error_from_errno(errno);
  }

  return err;
}

static int nfw_callback(const char *fpath, const struct stat *sb,
                        int typeflag) {
  if (typeflag == FTW_D) {
    return directory_delete(fpath);
  } else {
    return file_delete(fpath);
  }
}

