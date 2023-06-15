#include "image_load.h"
#include "sdl.h"
#include <png.h>

image_t image_load_png(char *path) {
  FILE *infile = fopen(path, "r");
  image_t empty = {0, 0, NULL};
  if (infile == NULL) {
    return empty;
  }

  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL) {
    return empty;
  }

  png_infop info = png_create_info_struct(png);
  if (info == NULL) {
    return empty;
  }

  png_init_io(png, infile);
  png_read_info(png, info);

  uint32_t width = png_get_image_width(png, info);
  uint32_t height = png_get_image_height(png, info);
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

  png_bytep *row_pointers = malloc(sizeof(png_bytep) * height);
  if (row_pointers == NULL) {
    png_destroy_read_struct(&png, &info, NULL);
    fclose(infile);
    return empty;
  }

  for (int i = 0; i < height; i++) {
    row_pointers[i] = malloc(png_get_rowbytes(png, info));
  }

  png_read_image(png, row_pointers);

  fclose(infile);
  png_destroy_read_struct(&png, &info, NULL);

  buffer_pixel_t *pixels =
      malloc(sizeof(buffer_pixel_t) * width * height);
  if (pixels == NULL) {
    png_destroy_read_struct(&png, &info, NULL);
    free(row_pointers);
    fclose(infile);
    return empty;
  }

  for (int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for (int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      pixels[y * width + x].r = px[0];
      pixels[y * width + x].g = px[1];
      pixels[y * width + x].b = px[2];
    }
  }

  image_t image = {.width = width, .height = height, .data = pixels};
  return image;
}
