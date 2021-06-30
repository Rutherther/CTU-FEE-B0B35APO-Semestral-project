#ifndef __MOUNT_UTILS_H__
#define __MOUNT_UTILS_H__

#include <sys/mount.h>
#include "file_access.h"

/**
 * @brief Mount device to target path
 * 
 * @param device device to be mounted
 * @param target folder to mount device to
 * @return file_operation_error_t
 */
file_operation_error_t device_mount(char *device, char *target);

/**
 * @brief Unmount device from target path
 *
 * @param device device to be unmounted
 * @param target folder device is mounted to
 * @return file_operation_error_t
 */
file_operation_error_t device_umount(char *device, char *target);

#endif // __MOUNT_UTILS_H__
