#ifndef CAB_IO_BUFFER
#define CAB_IO_BUFFER

#include <stdbool.h>
#include <stddef.h>

#define INITIAL_OUTPUT_BUFFER_ALLOCATED_SIZE 128


typedef struct {
    char* content;
    size_t current_size;
    size_t allocated_size;
} CAB_IOBuffer;

bool cab_io_buffer__is_initialized(CAB_IOBuffer buffer);
#endif
