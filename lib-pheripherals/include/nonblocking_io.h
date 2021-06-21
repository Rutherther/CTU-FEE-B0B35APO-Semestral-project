#ifndef _NONBLOCKING_IO_H
#define _NONBLOCKING_IO_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>

/**
 * @brief Set file to nonblocking using cfmakeraw
 * 
 * @param file to set
 * @param old termios struct to save old data to
 * @return int 
 */
int file_set_nonblocking(int file, struct termios *old);

/**
 * @brief Set file to old termios data
 * 
 * @param file to set
 * @param old termios struct to load old data from
 * @return int 
 */
int file_set_blocking(int file, struct termios *old);

/**
 * @brief Read from nonblocking file, if no data, return 0 instead of -1
 * 
 * @param file to read from
 * @param max_size size of buffer
 * @param data buffer to write data to
 * @return int 
 */
int file_read_nonblocking(int file, size_t max_size, uint8_t *data);

#endif //_NONBLOCKING_IO_H
