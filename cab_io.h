#ifndef CAB_IO
#define CAB_IO

#include <stddef.h>

#include "cab_string.h"

typedef enum {
   PRINT,
   TEXT
} OutputMode;


void output(const char* format_string, ...);
void io__setup();
void io__set_mode(OutputMode mode);
void io__shutdown(void);

String get_output();
#endif