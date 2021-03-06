#ifndef __FILE_EXECUTE_H__
#define __FILE_EXECUTE_H__

#include <stdbool.h>
#include <unistd.h>

#define WRITE_END 1
#define READ_END 0

/**
 * @brief This structure holds information about executing subprocess
 *
 */
typedef struct {
  pid_t pid;         /**< PID of the running process */
  int output_signal; /**< Exit code of the process*/
  bool exited;       /**< Whether the process has ended */

  int stderr_pipe[2];
} executing_file_t;

/**
 * @brief This structure holds error or executing subprocess
 *
 */
typedef struct {
  bool error;            /**< in case there was an error, this is true*/
  executing_file_t file; /**< executing file if error is false*/
} executing_file_error_t;

/**
 * @brief Start executing given file
 *
 * @param path path to the executable
 * @param args arguments to start the executable with
 * @return executing_file_error_t with either error or process information set
 */
executing_file_error_t executing_file_execute(char *path, char *args);

void executing_file_destroy(executing_file_t *file);

/**
 * @brief Wait for the process to exit
 *
 * @param file
 * @return int exit code
 */
int executing_file_wait(executing_file_t *file);

/**
 * @brief Check whether the process has ended and return immediatelly
 *
 * @param file
 * @return true when the process has ended
 * @return false when the process is still running
 */
bool executing_file_has_ended(executing_file_t *file);

#endif // __FILE_EXECUTE_H__
