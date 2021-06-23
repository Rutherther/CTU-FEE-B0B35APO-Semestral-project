#include "extern_file_access.h"
#include "device_mount.h"
#include "local_file_access.h"
#include <stdlib.h>
#include <string.h>

static bool extern_fileaccess_check_mount(fileaccess_state_t state) {
  return true;
}

fileaccess_state_t extern_fileaccess_init_state(void *data) {
  size_t device_length = strlen(data);

  extern_fileaccess_state_t externstate = {.path = "/mnt/file_browser_device",
                                           .mount_device = NULL,
                                           .mounted = false,
                                           .error = FILOPER_SUCCESS};

  char *malloced = malloc(sizeof(char) * (device_length + 1));
  if (malloced == NULL) {
    externstate.error = FILOPER_UNKNOWN;
  } else {
    strcpy(malloced, data);
    externstate.mount_device = malloced;
  }

  externstate.error = device_mount(externstate.mount_device, externstate.path);

  fileaccess_state_t state = {
      .fileaccess = &extern_file_access,
      .payload.exter = externstate,
  };

  return state;
}

bool extern_fileaccess_deinit_state(fileaccess_state_t state) {
  // unmount
  state.payload.exter.error =
      device_umount(state.payload.exter.mount_device, state.payload.exter.path);
  free(state.payload.exter.mount_device);

  return true;
}

directory_or_error_t extern_fileaccess_directory_list(fileaccess_state_t state,
                                                      char *path) {
  if (!extern_fileaccess_check_mount(state)) {
    directory_or_error_t error = {.error = true,
                                  .payload.error = FILOPER_UNKNOWN};

    return error;
  }

  return local_fileaccess_directory_list(state, path);
}

directory_or_error_t extern_fileaccess_root_list(fileaccess_state_t state) {
  if (!extern_fileaccess_check_mount(state)) {
    directory_or_error_t error = {.error = true,
                                  .payload.error = FILOPER_UNKNOWN};

    return error;
  }

  return local_fileaccess_root_list(state);
}

directory_or_error_t
extern_fileaccess_directory_create(fileaccess_state_t state, char *path) {
  if (!extern_fileaccess_check_mount(state)) {
    directory_or_error_t error = {.error = true,
                                  .payload.error = FILOPER_UNKNOWN};

    return error;
  }

  return local_fileaccess_directory_create(state, path);
}

file_operation_error_t
extern_fileaccess_directory_delete(fileaccess_state_t state, char *path) {
  if (!extern_fileaccess_check_mount(state)) {
    return FILOPER_UNKNOWN;
  }

  return local_fileaccess_directory_delete(state, path);
}

file_operation_error_t
extern_fileaccess_directory_close(fileaccess_state_t state,
                                  directory_t *directory) {
  if (!extern_fileaccess_check_mount(state)) {
    return FILOPER_UNKNOWN;
  }

  return local_fileaccess_directory_close(state, directory);
}

file_operation_error_t
extern_fileaccess_file_get_mime_type(fileaccess_state_t state, file_t *file,
                                     char *mime) {
  if (!extern_fileaccess_check_mount(state)) {
    return FILOPER_UNKNOWN;
  }

  return local_fileaccess_file_get_mime_type(state, file, mime);
}

executing_file_or_error_t
extern_fileaccess_file_execute(fileaccess_state_t state, file_t *file,
                               char *args) {
  if (!extern_fileaccess_check_mount(state)) {
    executing_file_or_error_t error = {.error = true,
                                       .payload.error = FILOPER_UNKNOWN};

    return error;
  }

  return local_fileaccess_file_execute(state, file, args);
}

file_operation_error_t extern_fileaccess_file_delete(fileaccess_state_t state,
                                                     char *path) {
  if (!extern_fileaccess_check_mount(state)) {
    return FILOPER_UNKNOWN;
  }

  return local_fileaccess_file_delete(state, path);
}

const fileaccess_t temp_file_access = {
    .type = FA_EXTERN,
    .init = extern_fileaccess_init_state,
    .deinit = extern_fileaccess_deinit_state,

    .delete_file = extern_fileaccess_file_delete,
    .execute_file = extern_fileaccess_file_execute,
    .get_mime_type = extern_fileaccess_file_get_mime_type,

    .list_directory = extern_fileaccess_directory_list,
    .close_directory = extern_fileaccess_directory_close,
    .create_directory = extern_fileaccess_directory_create,
    .delete_directory = extern_fileaccess_directory_delete,

    .list_root = extern_fileaccess_root_list,

};
