#ifndef CAB_OUTPUT_IN
#define CAB_OUTPUT_IN

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"
#include "cab_output_buffer.h"

void print_to_default_buffer(const char* text);

char* flush_output_buffer(void);

void start_message(OutputTags tags);

void end_message(void);

bool is_message_started(void);

void output__shutdown(void);

OutputBuffer get_tagged_output(void);

#endif
