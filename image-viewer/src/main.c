/*******************************************************************
  Image viewer (png, jpeg, ppm)
  made by František Boháček
 *******************************************************************/

#include "display_utils.h"
#include "image_viewer.h"

#include <png.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "nonblocking_io.h"
#include "logger.h"
#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"

int main(int argc, char *argv[])
{
  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");

    if (1) {
      printf("Waitting\n");
      /* Wait till application holding lock releases IT or exits */
      serialize_lock(0);
    }
  }

  struct termios oldstdin;
  file_set_nonblocking(STDIN_FILENO, &oldstdin);

  logger_t logger = logger_create(LOG_DEBUG, stdout, stdout, stderr, stderr, NULL);

  if (argc < 2) {
    logger_error(&logger, __FILE__, __FUNCTION__, __LINE__, "Not enough arguments.");
  }

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Image %s will be loaded.", argv[1]);

  display_data_t display_data = {
    .base_address = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0),
  };

  void *reg_knobs_base =
      map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0) +
      SPILED_REG_KNOBS_8BIT_o;

  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Initializing display...", argv[1]);
  display_t display = display_init(display_data);
  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Display initialized...", argv[1]);

  logger_debug(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Starting image viewer...", argv[1]);

  image_viewer_t viewer = image_viewer_create(argv[1], &display, &logger);

  image_viewer_display_image(&viewer);

  image_viewer_start_loop(&viewer, reg_knobs_base);

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__,
              "Cleaning up...", argv[1]);

  display_deinit(&display);
  image_viewer_destroy(&viewer);

  file_set_blocking(STDIN_FILENO, &oldstdin);

  /* Release the lock */
  serialize_unlock();

  logger_info(&logger, __FILE__, __FUNCTION__, __LINE__, "Application quit",
              argv[1]);

  return 0;
}
