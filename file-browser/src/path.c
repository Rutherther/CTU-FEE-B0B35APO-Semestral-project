#include "path.h"
#include <stdlib.h>
#include <string.h>

char *path_join(char *base, char *relative) {
  size_t base_len = strlen(base);
  size_t relative_len = strlen(relative);

  while (base[base_len - 1] == '/' && base_len > 0) {
    base_len--;
  }

  while (relative[0] == '/' && relative_len > 0) {
    relative_len--;
    relative++;
  }

  size_t new_len = base_len + relative_len + 1; // length of the string
  char *out = malloc((new_len + 1) * sizeof(out)); // length of the string plus one for \0
  out[new_len] = '\0';
  out[base_len] = '/';

  memcpy(out, base, base_len);
  memcpy(out + base_len + 1, relative, relative_len);

  return out;
}
