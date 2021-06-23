#include "file_access.h"
#include <stdbool.h>

int file_delete(const char *path);
int directory_delete(const char *path);

size_t file_get_full_path_memory_size(fileaccess_state_t state,
                                        directory_t *directory, file_t *file);
bool file_get_full_path(fileaccess_state_t state, directory_t *directory,
                        file_t *file, char *dest);
