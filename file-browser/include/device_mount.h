#ifndef __MOUNT_UTILS_H__
#define __MOUNT_UTILS_H__

#include <sys/mount.h>
#include "file_access.h"

file_operation_error_t device_mount(char *device, char *target);
file_operation_error_t device_umount(char *device, char *target);

char *device_mount_get_filesystem(char *device);

#endif // __MOUNT_UTILS_H__
