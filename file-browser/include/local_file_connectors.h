#ifndef __LOCAL_FILE_CONNECTORS_H__
#define __LOCAL_FILE_CONNECTORS_H__

#include "file_access.h"

file_operation_error_t local_connector_file_copy(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination);

file_operation_error_t local_connector_file_move(
    fileaccess_connector_t *connector, fileaccess_state_t first_state,
    fileaccess_state_t second_state, file_t *file, char *destination);

#endif // __LOCAL_FILE_CONNECTORS_H__
