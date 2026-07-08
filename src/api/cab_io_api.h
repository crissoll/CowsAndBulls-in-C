#ifndef CAB_IO_CORE
#define CAB_IO_CORE

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"


void io__setup(void); // must be called before using io functions
void io__shutdown(void); // must be called at the end of execution


InputStatus input(char *input_string);

char *get_output(void);

void update_output_messages();

char** get_messages_with_tag(OutputTags tag, size_t* message_count);


#endif
