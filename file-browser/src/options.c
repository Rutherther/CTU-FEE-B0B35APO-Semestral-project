#include "options.h"
#include "file_access.h"
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

exec_options_loader_t exec_options_loader_create(char *filename) {
  exec_options_loader_t loader = {
      .options_filename = filename,
      .bytes_size = 0,
      .exec_options = NULL,
  };

  return loader;
}

file_operation_error_t exec_options_loader_get_size(exec_options_loader_t *loader) {
  FILE *file = fopen(loader->options_filename, "r");
  if (file == NULL) {
    return file_operation_error_from_errno(errno);
  }
  int read = fread(&loader->bytes_size, sizeof(loader->bytes_size), 1, file);
  if (read < 1 && ferror(file)) {
    fclose(file);
    return file_operation_error_from_errno(errno);
  }

  fclose(file);
  return FILOPER_SUCCESS;
}

file_operation_error_t exec_options_loader_load(exec_options_loader_t *loader,
                                         char *buffer) {
  FILE *file = fopen(loader->options_filename, "r");
  if (file == NULL) {
    return file_operation_error_from_errno(errno);
  }

  fread(buffer, sizeof(char), loader->bytes_size, file);

  if (ferror(file)) {
    fclose(file);
    return file_operation_error_from_errno(errno);
  }

  fclose(file);
  loader->exec_options = (exec_options_t*)buffer;

  intptr_t buffer_ptr = (intptr_t)buffer;
  loader->exec_options->options += buffer_ptr;

  for (int i = 0; i < loader->exec_options->options_count; i++) {
    exec_option_t option = loader->exec_options->options[i];

    option.mime += buffer_ptr;
    option.program += buffer_ptr;
    loader->exec_options->options[i] = option;
  }

  return FILOPER_SUCCESS;
}

file_operation_error_t exec_options_save(exec_options_t *options, char *filename) {
  uint32_t length = sizeof(uint32_t);
  for (int i = 0; i < options->options_count; i++) {
    exec_option_t option = options->options[i];
    option.mime_length = strlen(option.mime);
    option.program_length = strlen(option.program);
    option.length = (option.mime_length + option.program_length) * sizeof(char) +
                sizeof(uint16_t) * 2;
    length += option.length;

    options->options[i] = option;
  }
  options->bytes_size = length;

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    return file_operation_error_from_errno(errno);
  }

  fwrite(&options->bytes_size, sizeof(options->bytes_size), 1, file);
  fwrite(&options->options_count, sizeof(options->options_count), 1, file);

  uint64_t offset = 1;
  fwrite(&offset, sizeof(offset), 1, file);

  if (ferror(file)) {
    fclose(file);
    return file_operation_error_from_errno(errno);
  }

  intptr_t chars_offset = (intptr_t) (sizeof(exec_option_t) * options->options_count);
  file_operation_error_t error = FILOPER_SUCCESS;
  for (int i = 0; i < options->options_count; i++) {
    exec_option_t option = options->options[i];
    fwrite(&option, sizeof(option.mime_length) + sizeof(option.length) + sizeof(option.program_length), 1, file);

    fwrite(&chars_offset, sizeof(chars_offset), 1, file);
    chars_offset += option.program_length + 1;

    fwrite(&chars_offset, sizeof(chars_offset), 1, file);
    chars_offset += option.mime_length + 1;

    if (ferror(file)) {
      fclose(file);
      return file_operation_error_from_errno(errno);
    }
  }

  for (int i = 0; i < options->options_count; i++) {
    exec_option_t option = options->options[i];
    fwrite(option.program, option.program_length + 1, 1, file);
    fwrite(option.mime, option.mime_length + 1, 1, file);

    if (ferror(file)) {
      fclose(file);
      return file_operation_error_from_errno(errno);
    }
  }

  fclose(file);
  return error;
}

char *exec_options_get_program(exec_options_t *options, char *mime) {
  for (int i = 0; i < options->options_count; i++) {
    exec_option_t option = options->options[i];
    if (strcmp(option.mime, mime) == 0) {
      return option.program;
    }
  }

  return NULL;
}
