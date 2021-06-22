#include <unistd.h>
#include <stdbool.h>

typedef struct {
  pid_t pid;
  int output_signal;
  bool exited;
} executing_file_t;

typedef struct {
  bool error;
  executing_file_t file;
} executing_file_error_t;

executing_file_error_t executing_file_execute(char *path, char *args);
int executing_file_wait(executing_file_t *file);
bool executing_file_has_ended(executing_file_t *file);
