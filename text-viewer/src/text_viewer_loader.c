#include "text_viewer_loader.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

static file_error_t file_error_from_errno() {
  switch (errno) {
  case ENOENT:
    return FILER_NOT_FOUND;
  case EACCES:
    return FILER_NO_PERMISSIONS;
  default:
    return FILER_FILE_CANT_OPEN;
  }
}

file_error_t text_viewer_load_file(text_viewer_t *text_viewer) {
  FILE *file = fopen(text_viewer->path, "r");

  if (file == NULL) {
    return file_error_from_errno();
  }

  fseek(file, 0, SEEK_END);
  long fsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *data = malloc(sizeof(char) * (fsize + 2));
  data[fsize - 1] = '\0';
  data[fsize] = '\0';
  data[fsize + 1] = '\0';

  if (data == NULL) {
    return FILER_UNKNOWN;
  }

  long read = 0;
  const int perc = 5;
  const int iters = 100 / perc;
  for (int i = 0; i < iters; i++) {
    long to_read = fsize / iters;
    if (to_read == 0) {
      i = iters - 1;
    }

    if (i == iters - 1) {
      to_read = fsize - read - 1;
    }

    if (to_read == 0 || fsize == read) {
      break;
    }

    long result = fread(data + read, sizeof(char), to_read, file);
    read += result;

    if (result != to_read) {
      fclose(file);
      return FILER_CANNOT_READ;
    }

    ledstrip_progress_bar_step(text_viewer->pheripherals.ledstrip, i * perc);
  }

  fclose(file);

  multiline_text_t *text =
      gui_multiline_text_create(&text_viewer->font, WHITE_PIXEL, data);
  if (text == NULL) {
    return FILER_UNKNOWN;
  }

  text_viewer->multiline_text = text;

  ledstrip_clear(text_viewer->pheripherals.ledstrip);
  return FILER_SUCCESS;
}
