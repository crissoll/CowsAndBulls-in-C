#include "cab_output_buffer.h"

static OutputBuffer default_output_buffer = (OutputBuffer){0};

char* flush_output_buffer(void) {
    return output_buffer__flush(&default_output_buffer);
}

void output__shutdown(void) {
    cab_output_buffer__free_content(default_output_buffer);
    default_output_buffer = (OutputBuffer){0};
}

void print_to_default_buffer(const char* text) {
    if (!cab_output_buffer__is_initialized(default_output_buffer)) {
        cab_output_buffer__init(&default_output_buffer);
    }
    print_to_buffer(default_output_buffer.text_buffer, text);
}

OutputBuffer get_tagged_output(void) {
    return output_buffer__get_tagged_output(&default_output_buffer);
}


void start_message(OutputTags tags) {
    output_buffer__start_message(&default_output_buffer, tags);
}

void end_message(void) {
    output_buffer__end_message(&default_output_buffer);
}


bool is_message_started(void) {
    return output_buffer__is_message_started(default_output_buffer);
}
