#include <malloc.h>
#include <stdlib.h>

#include "cab_errors.h"

#include "cab_io_buffer.h"

bool cab_io_buffer__is_initialized(CAB_IOBuffer buffer) {
    return buffer.allocated_size > 0 && buffer.content != NULL;
}

void cab_io_buffer__init(CAB_IOBuffer* buffer) {
    if (buffer == NULL) {
        extra_io_warning("cab_io_buffer__init: passed NULL buffer argument\n");
        return;
    }
    buffer->content =
        realloc(buffer->content, sizeof(buffer->content[0]) *
                                     INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE);
    if (buffer->content == NULL) {
        extra_io_warning("cab_io_buffer__init: malloc failure\n");
        buffer->allocated_size = 0;
        buffer->current_size = 0;
        return;
    }

    buffer->allocated_size = INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE;
    buffer->current_size = 0;
    buffer->content[0] = '\0';
}

void cab_io_buffer__free_content(CAB_IOBuffer* buffer) {
    if (buffer == NULL) {
        return;
    }
    free(buffer->content);
    buffer->content = NULL;
    buffer->allocated_size = 0;
    buffer->current_size = 0;
}
