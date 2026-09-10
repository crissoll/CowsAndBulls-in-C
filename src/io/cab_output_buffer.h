#ifndef CAB_OUTPUT_BUFFER
#define CAB_OUTPUT_BUFFER

#include <stdbool.h>
#include <stddef.h>

#include "cab_io_buffer.h"
#include "cab_io_consts.h"

typedef struct CabSession CabSession;
typedef struct {
    size_t* message_indexes;
    OutputTags* tags;
    size_t size;
    CAB_IOBuffer* text_buffer;
} OutputBuffer;


bool cab_output_buffer__is_initialized(OutputBuffer messages);

void cab_output_buffer__init(OutputBuffer* messages);

char* output_buffer__flush(OutputBuffer* output_buffer);

void cab_output_buffer__free_content(OutputBuffer output_buffer);

void print_to_buffer(CAB_IOBuffer* buffer, const char* text);

OutputBuffer output_buffer__get_tagged_output(OutputBuffer* output_buffer);

void output_buffer__start_message(OutputBuffer* output_buffer, OutputTags tags);

void output_buffer__end_message(OutputBuffer* output_buffer);

bool output_buffer__is_message_started(OutputBuffer output_buffer);


void log_tagged_output(CabSession* session);
#endif
