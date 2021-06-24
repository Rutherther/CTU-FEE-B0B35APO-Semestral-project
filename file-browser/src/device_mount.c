#include "device_mount.h"
#include "file_access.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

file_operation_error_t device_mount(char *device, char *target) {
  // TODO: change to use mount(), not system()
  char buffer[strlen(device) + strlen(target) + 20];
  snprintf(buffer, sizeof(buffer), "mount -t auto %s %s", device, target);
  system(buffer);
  return FILOPER_SUCCESS;
}

file_operation_error_t device_umount(char *device, char *target) {
  // TODO: change to use mount(), not system()
  char buffer[strlen(device) + strlen(target) + 20];
  snprintf(buffer, sizeof(buffer), "umount %s", target);
  system(buffer);
  return FILOPER_SUCCESS;
}
