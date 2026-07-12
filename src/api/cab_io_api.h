#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"


// must be called at the end of execution to avoid memory leaks
void cab_io_shutdown(); 

// clear the inner buffer and insert input_string into it.
// if it's called twice without processing it (with session functions) the first input will be discarded
InputStatus cab_input(const char *input_string);

// returns a simple stream of all messages
char *cab_get_output();

// returns all the messages with the specified tag
char** cab_get_messages_with_tag(OutputTags tag, size_t* message_count);

#endif
