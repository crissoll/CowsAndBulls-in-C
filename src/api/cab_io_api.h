#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"


void io__reset();
void io__shutdown(); // must be called at the end of execution to avoid memory leaks


InputStatus input(const char *input_string);

char *get_output();

char** get_messages_with_tag(OutputTags tag, size_t* message_count);

#endif
