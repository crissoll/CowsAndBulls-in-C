#ifndef CAB_OUTPUT
#define CAB_OUTPUT

#include "api/cab_io_api.h"

void io__set_output_mode(OutputMode new_mode);
void output(const char* format_string, ...);

#endif
