#include "file_access.h"
#include <stdbool.h>
#include "dirent.h"

int file_delete(const char *path);
int directory_delete(const char *path);

size_t file_get_full_path_memory_size(fileaccess_state_t state,
                                        directory_t *directory, file_t *file);
bool file_get_full_path(fileaccess_state_t state, directory_t *directory,
                        file_t *file, char *dest);

file_operation_error_t file_get_information(void *malloced,
                                            uint64_t *file_offset,
                                            uint64_t *names_offset,
                                            fileaccess_state_t state,
                                            file_t file);

file_operation_error_t directory_list(fileaccess_state_t state, void *malloced,
                                      char *show_path, uint32_t files_count,
                                      DIR *dirptr, directory_t *directory);
