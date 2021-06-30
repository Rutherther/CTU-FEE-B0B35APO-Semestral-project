#include "path.h"
#include <stdlib.h>
#include <string.h>

size_t path_join_memory_size(char *base, char *relative) {
  return strlen(base) + strlen(relative) + 1;
}

bool path_join(char *base, char *relative, char *out) {
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
  out[new_len] = '\0';
  out[base_len] = '/';

  memcpy(out, base, base_len);
  memcpy(out + base_len + 1, relative, relative_len);

  return true;
}
