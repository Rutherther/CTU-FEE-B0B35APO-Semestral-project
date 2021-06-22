#include "file_execute.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

executing_file_error_t executing_file_execute(char *path, char *args) {
  executing_file_error_t ret;
  pid_t pid = fork();

  if (pid == -1) {
    ret.error = true;
    return ret;
  }

  if (pid == 0) {
    execl(path, args, (char*)NULL);
    exit(errno);
  }

  ret.error = false;
  ret.file.pid = pid;
  ret.file.output_signal = 0;
  ret.file.exited = false;
  return ret;
}

int executing_file_wait(executing_file_t *file) {
  pid_t data = waitpid(file->pid, &file->output_signal, 0);
  if (data == -1) {
    return -1;
  }

  file->exited = true;
  return file->output_signal;
}

bool executing_file_has_ended(executing_file_t *file) {
  if (file->exited) {
    return true;
  }

  if (waitpid(file->pid, &file->output_signal, WNOHANG) == 0) {
    file->exited = true;
    return true;
  }

  return false;
}
