#include "file_execute.h"
#include "file_access.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

executing_file_error_t executing_file_execute(char *path, char *args) {
  executing_file_error_t ret;
  executing_file_t file;

  if (pipe(file.stderr_pipe) == -1) {
    ret.error = file_operation_error_from_errno(errno);
    return ret;
  }

  pid_t pid = fork();

  if (pid == -1) {
    close(file.stderr_pipe[0]);
    close(file.stderr_pipe[1]);

    ret.error = true;
    return ret;
  }

  if (pid == 0) { // child process
    close(file.stderr_pipe[READ_END]);

    if(dup2(file.stderr_pipe[WRITE_END], STDERR_FILENO) == -1) {
      perror("There was an error during dup2");
      exit(errno);
    }

    execl(path, path, args, (char*)NULL);

    // Is reached only in case of an error
    fprintf(stderr, "Could not execute file: %s\r\n",
            fileaccess_get_error_text(file_operation_error_from_errno(errno)));
    exit(errno);
  }

  // parent process
  close(file.stderr_pipe[WRITE_END]);

  ret.error = false;
  ret.file = file;
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

void executing_file_destroy(executing_file_t *file) {
  close(file->stderr_pipe[READ_END]);
}
