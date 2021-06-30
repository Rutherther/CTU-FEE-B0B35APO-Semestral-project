#include "file_browser_utils.h"
#include "dialog.h"

#define STDERR_BUFFER_LENGTH 3000

void
file_browser_handle_opened_file(opened_file_state_t opened, gui_t *gui,
                                font_t *font) {
  logger_t *logger = gui->logger;
  if (opened.error != FILOPER_SUCCESS) {
    fileaccess_log_error(logger, opened.error);
    dialog_info_show(gui, font, "Could not open file",
                     fileaccess_get_error_text(opened.error));
  } else if (opened.executed) {
    if (opened.ended_with_error) {
      logger_error(logger, __FILE__, __FUNCTION__, __LINE__,
                   "Executed file returned unhealthy signal %d",
                   opened.executing_file.output_signal);

      char buff[STDERR_BUFFER_LENGTH];
      int chars_read = read(opened.executing_file.stderr_pipe[READ_END], buff,
                            STDERR_BUFFER_LENGTH);
      buff[chars_read] = '\0';

      logger_error(logger, __FILE__, __FUNCTION__, __LINE__,
                   "Returned stderr: %s", buff);
      dialog_info_show(gui, font, "Exited with nonzero code",
                       buff);
    } else {
      logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                  "Successfully returned from executing file.");
    }

    executing_file_destroy(&opened.executing_file);
  } else {
    logger_info(logger, __FILE__, __FUNCTION__, __LINE__,
                "Successfully returned without executing anything.");
  }
}
