#include "image_loader.h"
#include "display_utils.h"

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <magic.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LENGTH 4

image_error_t image_loader_load(image_t *image) {
  image_error_t error = image_deduce_type(image);
  if (error != IMERR_SUCCESS) {
    return error;
  }

  switch (image->type) {
  case IMG_PPM:
    return image_loader_load_ppm(image);
  case IMG_JPG:
    return image_loader_load_jpeg(image);
  case IMG_PNG:
    return image_loader_load_png(image);
  case IMG_UNKNOWN:
    return IMERR_UNKNOWN_FORMAT;
  }

  return IMERR_UNKNOWN;
}

image_error_t image_error_from_errno() {
  switch (errno) {
  case ENOENT:
    return IMERR_FILE_NOT_FOUND;
  case EACCES:
    return IMERR_FILE_NO_PERMISSIONS;
  default:
    return IMERR_FILE_CANT_OPEN;
  }
}

image_error_t image_loader_load_ppm(image_t *image) {
  FILE *file = fopen(image->path, "r");
  if (file == NULL) {
    return image_error_from_errno();
  }

  char null[10];
  short maxBrightness;

  if (fscanf(file, "%2s %hd %hd %hd", null, &image->width, &image->height, &maxBrightness) != 4) {
    return IMERR_WRONG_FORMAT;
  }

  fseek(file, 1, SEEK_CUR);

  raw_pixel_onebit_t max = {.red = maxBrightness, .green = maxBrightness, .blue = maxBrightness};

  image->pixels = malloc(image->width * image->height * sizeof(display_pixel_t));
  if (image->pixels == NULL) {
    return IMERR_UNKNOWN;
  }

  for (int i = 0; i < image->height * image->width; i++) {
    raw_pixel_onebit_t pixel;
    if (fread(&pixel, sizeof(raw_pixel_onebit_t), 1, file) < 1) {
      free(image->pixels);
      fclose(file);
      return IMERR_UNKNOWN;
    }

    image->pixels[i] = raw_pixel_onebit_convert_to_display(pixel, max);
  }

  fclose(file);
  return IMERR_SUCCESS;
}


image_error_t image_loader_load_jpeg(image_t *image) {
  return IMERR_WRONG_FORMAT;
}

image_error_t image_loader_load_png(image_t *image) {
  return IMERR_WRONG_FORMAT;
}

image_error_t image_deduce_type(image_t *image) {
  FILE *file = fopen(image->path, "r");
  if (file == NULL) {
    return image_error_from_errno();
  }

  uint8_t data[BUFFER_LENGTH];
  if (fread(data, 1, BUFFER_LENGTH, file) != BUFFER_LENGTH) {
    fclose(file);
    return IMERR_UNKNOWN;
  }

  fclose(file);

  if (data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) {
    image->type = IMG_JPG;
  } else if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E &&
             data[3] == 0x47) {
    image->type = IMG_PNG;
  } else if (data[0] == 'P' && data[1] == '6') {
    image->type = IMG_PPM;
  } else {
    return IMERR_UNKNOWN_FORMAT;
  }

  return IMERR_SUCCESS;
}
