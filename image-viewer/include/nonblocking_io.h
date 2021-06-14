#ifndef _NONBLOCKING_IO_H
#define _NONBLOCKING_IO_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>

int file_set_nonblocking(int file, struct termios *old);
int file_set_blocking(int file, struct termios *old);
int file_read_nonblocking(int file, size_t max_size, uint8_t *data);
bool file_write_nonblocking(int file, size_t size, uint8_t *data, int max_delay);

#endif //_NONBLOCKING_IO_H
