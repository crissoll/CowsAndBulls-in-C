#ifndef CAB_OUTPUT_IN
#define CAB_OUTPUT_IN

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"

void print_to_default_buffer(const char* text);

char* get_output();

void output__setup();

void output__shutdown();

void start_message(OutputTags tags);

void end_message();

bool is_message_started();

Messages get_messages_tags();

#endif