#ifndef CAB_OUTPUT_BUFF_VIEW
#define CAB_OUTPUT_BUFF_VIEW

#include <stddef.h>

typedef struct CabSession CabSession;

typedef struct {
    char** messages;
    size_t count;
} CabOutputBufferViewEntry;

typedef struct {
    CabOutputBufferViewEntry* entries;
    size_t size;
} CabOutputBufferView;

void cab_session__free_output_buffer_view(CabSession* session);

#endif
