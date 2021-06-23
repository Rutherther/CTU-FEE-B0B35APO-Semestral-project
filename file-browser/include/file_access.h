#ifndef __FILE_ACCESS_H__
#define __FILE_ACCESS_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include "file_execute.h"

typedef struct directory_t directory_t;

typedef enum {
  FT_FOLDER,
  FT_FILE,
  FT_OTHER,
  FT_UNKNOWN,
} filetype_t;

typedef enum {
  FA_LOCAL,  // root fs
  FA_EXTERN, // not mounted device to be mounted when opened
  FA_TEMP,
  FA_COUNT,
} fileaccess_type_t;

typedef struct {
  char *name;
  directory_t *directory;
  filetype_t type;

  // stats
  uint64_t size;
  time_t modify_time;

  mode_t permissions;
  uid_t uid;
  gid_t gid;
} file_t;

struct directory_t {
  char *path;
  file_t *files;
  uint32_t files_count;
};

typedef enum {
  FILOPER_SUCCESS,
  FILOPER_PERMISSIONS,
  FILOPER_DOES_NOT_EXIST,
  FILOPER_USED,
  FILOPER_ALREADY_EXISTS,
  FILOPER_NOT_ENOUGH_SPACE,
  FILOPER_UNKNOWN,
} file_operation_error_t;

typedef struct {
  bool error;
  union {
    file_operation_error_t error;
    executing_file_t file;
  } payload;
} executing_file_or_error_t;

typedef struct {
  bool error;
  union {
    file_operation_error_t error;
    directory_t *directory;
  } payload;
} directory_or_error_t;

typedef struct fileaccess_t fileaccess_t;

typedef struct {
  char *path;
} local_fileaccess_state_t;

typedef struct {
  char *path; // ORDER MATTERS!
  char *mount_device;
  bool mounted;
  file_operation_error_t error;
} extern_fileaccess_state_t;

typedef local_fileaccess_state_t temp_fileaccess_state_t;

typedef struct {
  const fileaccess_t *fileaccess;
  union {
    local_fileaccess_state_t local;
    local_fileaccess_state_t temp;
    extern_fileaccess_state_t exter;
  } payload;
} fileaccess_state_t;

typedef fileaccess_state_t (*init_state_fn)(void *data);
typedef bool (*deinit_state_fn)(fileaccess_state_t state);

typedef directory_or_error_t (*list_directory_fn)(fileaccess_state_t state,
                                                  char *path);
typedef directory_or_error_t (*list_root_fn)(fileaccess_state_t state);

typedef directory_or_error_t (*create_directory_fn)(fileaccess_state_t state,
                                                    char *path);
typedef file_operation_error_t (*close_directory_fn)(fileaccess_state_t state,
                                                     directory_t *directory);

typedef file_operation_error_t (*get_mime_type_fn)(fileaccess_state_t state,
                                                   file_t *file, char *mime);
typedef executing_file_or_error_t (*execute_file_fn)(fileaccess_state_t state,
                                          file_t *file, char *args);

typedef file_operation_error_t (*delete_directory_fn)(fileaccess_state_t state,
                                                      char *path);
typedef file_operation_error_t (*delete_file_fn)(fileaccess_state_t state,
                                                 char *path);

struct fileaccess_t {
  fileaccess_type_t type;

  init_state_fn init;
  deinit_state_fn deinit;

  list_directory_fn list_directory;
  list_root_fn list_root;

  create_directory_fn create_directory;
  close_directory_fn close_directory;

  get_mime_type_fn get_mime_type;
  execute_file_fn execute_file;

  delete_directory_fn delete_directory;
  delete_file_fn delete_file;
};

typedef struct fileaccess_connector_t fileaccess_connector_t;
typedef file_operation_error_t (*file_copy_fn)(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination);
typedef file_operation_error_t (*file_move_fn)(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination);

struct fileaccess_connector_t {
  const fileaccess_t *first;
  const fileaccess_t *second;

  file_copy_fn copy;
  file_move_fn move;
};

extern const fileaccess_t
    local_file_access; // state is root directory descriptor
extern const fileaccess_t
    extern_file_access; // state is mount directory descriptor
extern const fileaccess_t
    temp_file_access; // state is /tmp directory descriptor

extern uint8_t fileaccess_connectors_count;
extern fileaccess_connector_t fileaccess_connectors[(FA_COUNT-1)*FA_COUNT];

fileaccess_state_t fileaccess_init(const fileaccess_t *fileaccess, void *data);
bool fileaccess_deinit(fileaccess_state_t state);

directory_or_error_t fileaccess_directory_list(fileaccess_state_t state,
                                               char *path);

directory_or_error_t fileaccess_root_list(fileaccess_state_t state);

directory_or_error_t fileaccess_directory_create(fileaccess_state_t state,
                                                 char *path);
file_operation_error_t fileaccess_directory_close(fileaccess_state_t state,
                                                  directory_t *directory);

file_operation_error_t fileaccess_directory_delete(fileaccess_state_t state,
                                                  char *path);

file_operation_error_t fileaccess_file_get_mimetype(fileaccess_state_t state,
                                                    file_t *file,
                                                    /*out*/ char *mime);

executing_file_or_error_t fileaccess_file_execute(fileaccess_state_t state, file_t *file,
                                       char *args);
file_operation_error_t fileaccess_file_delete(fileaccess_state_t state, char *path);

file_operation_error_t file_operation_error_from_errno(int error);

#endif // __FILE_ACCESS_H__
