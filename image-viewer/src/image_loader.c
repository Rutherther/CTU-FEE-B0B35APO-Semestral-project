#include "image_loader.h"
#include "display_utils.h"

#include <asm-generic/errno-base.h>
#include <errno.h>
#include <jpeglib.h>
#include <magic.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 4

image_error_t image_loader_load(image_t *image, image_load_callback callback, void *state) {
  image_error_t error = image_deduce_type(image);
  if (error != IMERR_SUCCESS) {
    return error;
  }

  switch (image->type) {
  case IMG_PPM:
    return image_loader_load_ppm(image, callback, state);
  case IMG_JPG:
    return image_loader_load_jpeg(image, callback, state);
  case IMG_PNG:
    return image_loader_load_png(image, callback, state);
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

image_error_t image_loader_load_ppm(image_t *image,
                                    image_load_callback callback, void *state) {
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
    callback(state, (double)i / (image->height * image->width));
  }

  fclose(file);
  return IMERR_SUCCESS;
}

image_error_t image_loader_load_jpeg(image_t *image,
                                     image_load_callback callback,
                                     void *state) {
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  JSAMPROW row_pointer[1];

  FILE *infile = fopen(image->path, "r");
  if (infile == NULL) {
    return image_error_from_errno();
  }

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  // handle errors

  image->width = cinfo.image_width;
  image->height = cinfo.image_height;

  int image_size = cinfo.image_width * cinfo.image_height * 2;
  unsigned char *out = malloc(image_size);
  if (out == NULL) {
    return IMERR_UNKNOWN;
    fclose(infile);
  }

  cinfo.out_color_space = JCS_RGB565;
  jpeg_start_decompress(&cinfo);

  while (cinfo.output_scanline < cinfo.output_height) {
    row_pointer[0] = &out[cinfo.output_scanline * cinfo.image_width * 2];
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
    callback(state, (double)cinfo.output_scanline / cinfo.output_height);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);
  
  image->pixels = (display_pixel_t*)out;
  return IMERR_SUCCESS;
}

image_error_t image_loader_load_png(image_t *image,
                                    image_load_callback callback, void *state) {
  FILE *infile = fopen(image->path, "r");
  if (infile == NULL) {
    return image_error_from_errno();
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    return IMERR_UNKNOWN;
  }

  png_infop info = png_create_info_struct(png);
  if (info == NULL) {
    return IMERR_WRONG_FORMAT;
  }

  png_init_io(png, infile);
  png_read_info(png, info);

  image->width = png_get_image_width(png, info);
  image->height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);

  if (bit_depth == 16) {
    png_set_strip_16(png);
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_palette_to_rgb(png);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
    png_set_expand_gray_1_2_4_to_8(png);
  }

  if (png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
  }

  if (color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }

  png_read_update_info(png, info);

  png_bytep *row_pointers = malloc(sizeof(png_bytep) * image->height);
  if (row_pointers == NULL) {
    png_destroy_read_struct(&png, &info, NULL);
    fclose(infile);
    return IMERR_UNKNOWN;
  }

  for (int i = 0; i < image->height; i++) {
    row_pointers[i] = malloc(png_get_rowbytes(png, info));
    callback(state, 0.5 * ((double)i / image->height));
  }

  png_read_image(png, row_pointers);

  fclose(infile);
  png_destroy_read_struct(&png, &info, NULL);

  display_pixel_t *pixels = malloc(sizeof(display_pixel_t) * image->width * image->height);
  if (pixels == NULL) {
    png_destroy_read_struct(&png, &info, NULL);
    free(row_pointers);
    fclose(infile);
    return IMERR_UNKNOWN;
  }

  for (int y = 0; y < image->height; y++) {
    png_bytep row = row_pointers[y];
    for (int x = 0; x < image->width; x++) {
      png_bytep px = &(row[x * 4]);
      int alpha = px[3];

      int coef = 255 * 255;
      pixels[y * image->width + x].fields.r = ((double)px[0] * alpha / coef) * DISPLAY_MAX_RED;
      pixels[y * image->width + x].fields.g = ((double)px[1] * alpha / coef) * DISPLAY_MAX_GREEN;
      pixels[y * image->width + x].fields.b = ((double)px[2] * alpha / coef) * DISPLAY_MAX_BLUE;
    }

    callback(state, 0.5 + 0.5 * ((double)y / image->height));
  }

  image->pixels = pixels;

  return IMERR_SUCCESS;
}

image_error_t image_deduce_type(image_t *image) {
  FILE *file = fopen(image->path, "r");
  if (file == NULL) {
    return image_error_from_errno();
  }

  magic_t magic = magic_open(MAGIC_MIME_TYPE);
  magic_load(magic, NULL);

  const char *mime = magic_file(magic, image->path);

  if (strstr(mime, "jpeg") != NULL || strstr(mime, "jpg") != NULL) {
    image->type = IMG_JPG;
  } else if (strstr(mime, "image/png") != NULL) {
    image->type = IMG_PNG;
  } else {
    uint8_t data[BUFFER_LENGTH];
    if (fread(data, 1, BUFFER_LENGTH, file) != BUFFER_LENGTH) {
      fclose(file);
      return IMERR_UNKNOWN;
    }

    fclose(file);

    if (data[0] == 'P' && data[1] == '6') {
      image->type = IMG_PPM;
    } else {
      magic_close(magic);
      return IMERR_UNKNOWN_FORMAT;
    }
  }

  magic_close(magic);
  return IMERR_SUCCESS;
}
