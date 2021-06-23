#ifndef __PATH_H__
#define __PATH_H__

#include <string.h>
#include <stdbool.h>

size_t path_join_memory_size(char *base, char *relative);
bool path_join(char *base, char *relative, char *out);

#endif // __PATH_H__
