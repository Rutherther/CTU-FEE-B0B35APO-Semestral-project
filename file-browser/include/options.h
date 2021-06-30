#include "file_access.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint16_t length;
  uint16_t mime_length;
  uint16_t program_length;
  char *mime;
  char *program;
} __attribute__((__packed__)) exec_option_t;

typedef struct {
  uint32_t bytes_size;
  uint16_t options_count;
  exec_option_t *options;
} __attribute__((__packed__)) exec_options_t;

typedef struct {
  char *options_filename;
  uint32_t bytes_size;
  exec_options_t *exec_options;
} exec_options_loader_t;

exec_options_loader_t exec_options_loader_create(char *filename);

file_operation_error_t exec_options_loader_get_size(exec_options_loader_t *loader);
file_operation_error_t exec_options_loader_load(exec_options_loader_t *loader, char *buffer);

file_operation_error_t exec_options_save(exec_options_t *options, char *filename);
char *exec_options_get_program(exec_options_t *options, char *mime);

extern exec_options_t *browser_exec_options;
