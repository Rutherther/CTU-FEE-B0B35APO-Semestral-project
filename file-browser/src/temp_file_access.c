#include "temp_file_access.h"
#include "local_file_access.h"
#define TMP_PATH "/tmp"

fileaccess_state_t temp_fileaccess_init_state(void *data) {
  fileaccess_state_t state = {.payload.temp.path = TMP_PATH,
                              .fileaccess = &temp_file_access};
  return state;
}

bool temp_fileaccess_deinit_state(fileaccess_state_t state) {
  // do nothing
  return true;
}

const fileaccess_t temp_file_access = {
    .type = FA_LOCAL,
    .init = temp_fileaccess_init_state,
    .deinit = temp_fileaccess_deinit_state,

    .delete_file = local_fileaccess_file_delete,
    .execute_file = local_fileaccess_file_execute,
    .get_mime_type = local_fileaccess_file_get_mime_type,
    .get_file_local_path = local_fileaccess_file_get_local_path,

    .list_directory = local_fileaccess_directory_list,
    .close_directory = local_fileaccess_directory_close,
    .create_directory = local_fileaccess_directory_create,
    .delete_directory = local_fileaccess_directory_delete,

    .list_root = local_fileaccess_root_list,
};
