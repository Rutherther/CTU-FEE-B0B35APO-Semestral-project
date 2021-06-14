#include "nonblocking_io.h"
#include "time_measure.h"
#include <stdint.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

int file_set_blocking(int file, struct termios *old) {
  int oldfl;
  oldfl = fcntl(file, F_GETFL);
  if (oldfl == -1) {
    return oldfl;
  }

  if(tcsetattr(file, TCSANOW, old) == -1) {
    return -1;
  }

  return fcntl(file, F_SETFL, oldfl & ~O_NONBLOCK);
}

int file_set_nonblocking(int file, struct termios *old)
{
  fcntl(file, F_SETFL, O_NONBLOCK);

  struct termios attrs;
  if (tcgetattr(file, &attrs) < 0) {
    return -1;
  }

  if (old != NULL) {
    tcgetattr(file, old);
  }

  cfmakeraw(&attrs);

  tcsetattr(file, TCSANOW, &attrs);
  return 1;
}

int file_read_nonblocking(int file, size_t max_size, uint8_t *data)
{
  int read_bytes = read(file, data, max_size);

  int error = errno;
  if (read_bytes == -1 && error == EAGAIN) {
    read_bytes = 0; // Do not treat EAGAIN as an error.
  } else {
    errno = error;
  }

  return read_bytes;
}

bool file_write_nonblocking(int file, size_t size, uint8_t *data, int max_delay) {
  int written = 0;
  bool correct = true;

  TimeMeasure measure = tmeasure_start();

  while (written < size && !tmeasure_exceededmilli(&measure, max_delay)) {
    int status = write(file, data + written, size - written);

    if (status == -1) {
      int error = errno;

      if (error != EAGAIN) {
        errno = error;
        correct = false;
        break;
      }
    } else {
      written += status;
    }
  }

  if (correct && written < size) {
    errno = ETIMEDOUT;
  }

  return correct;
}
