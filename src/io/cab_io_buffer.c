#include "cab_io_buffer.h"


bool cab_io_buffer__is_initialized(CAB_IOBuffer buffer) {
    return buffer.allocated_size > 0 && buffer.buffer != NULL;
}
