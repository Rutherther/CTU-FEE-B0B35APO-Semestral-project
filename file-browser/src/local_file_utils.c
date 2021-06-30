#include "local_file_utils.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "path.h"
#include <errno.h>
#include <ftw.h>
#include <string.h>
#include <stdio.h>
#include "dirent.h"

static int nfw_callback(const char *fpath, const struct stat *sb, int typeflag);

size_t file_get_full_path_memory_size(fileaccess_state_t state, directory_t *directory, file_t *file) {
  size_t root = strlen(state.payload.local.path);
  size_t dir = strlen(directory->path);
  size_t file_name = strlen(file->name);

  return root + 1 + dir + 1 + file_name + 1;
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


file_operation_error_t file_get_information(void *malloced,
                                                   uint64_t *file_offset,
                                                   uint64_t *names_offset,
                                                   fileaccess_state_t state,
                                                   file_t file) {
  size_t name_len = strlen(file.name);

  char full_path[file_get_full_path_memory_size(state, file.directory, &file)];
  file_get_full_path(state, file.directory, &file, full_path);

  // load info
  struct stat stats;
  int status = stat(full_path, &stats);

  if (status == -1) {
    file.size = 0;
    file.gid = 0;
    file.uid = 0;
    file.permissions = 0;
    file.modify_time = 0;
  } else {
    file.size = stats.st_size;
    file.gid = stats.st_gid;
    file.uid = stats.st_uid;
    file.permissions = stats.st_mode;
    file.modify_time = stats.st_mtim.tv_sec;
  }

  file_t *stored = malloced + *file_offset;
  *stored = file;
  *file_offset += sizeof(file_t);

  strcpy(malloced + *names_offset, file.name);
  stored->name = malloced + *names_offset;
  *names_offset += name_len + 1;

  return FILOPER_SUCCESS;
}

file_operation_error_t directory_list(fileaccess_state_t state, void *malloced,
                                      char *show_path, uint32_t files_count,
                                      DIR *dirptr, directory_t *directory) {
  uint64_t files_offset = sizeof(directory_t) + strlen(show_path) + 1;
  uint64_t names_offset = files_count * sizeof(file_t) + files_offset;

  file_operation_error_t error = FILOPER_SUCCESS;
  directory->path = malloced + sizeof(directory_t);
  directory->files = malloced + files_offset;
  directory->files_count = 0;
  strcpy(directory->path, show_path);

  struct dirent *dir;
  errno = 0;
  while ((dir = readdir(dirptr)) != NULL) {
    file_t file;
    file.directory = directory;
    file.name = dir->d_name;

    switch (dir->d_type) {
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

    error = file_get_information(malloced, &files_offset,
                                 &names_offset, state, file);
    errno = 0;
    if (error != FILOPER_SUCCESS) {
      return error;
    }

    directory->files_count++;
  }

  closedir(dirptr);
  return error;
}
