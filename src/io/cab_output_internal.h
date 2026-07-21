#ifndef CAB_OUTPUT_IN
#define CAB_OUTPUT_IN

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_consts.h"

typedef struct {
    size_t* messages;
    OutputTags* tags;
    size_t size;
} Messages;

void print_to_default_buffer(const char* text);

char* flush_output_buffer();

void start_message(OutputTags tags);

void end_message();

bool is_message_started();

Messages get_messages_tags();

void output__shutdown();

void set_log_messages_from_size_t(size_t value);

#endif
