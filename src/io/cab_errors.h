#ifndef CAB_ERRORS
#define CAB_ERRORS

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

typedef enum {
    LOG_None = 0,
    LOG_ToFile = 1,
    LOG_ToStdout = 2,
} LogMode;

void reset_extra_io_log(void);

void extra_io_warning(const CabSession* session, const char* warning_message,
                      ...);

#endif
