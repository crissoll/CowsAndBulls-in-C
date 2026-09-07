#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include "cab_output_buffer.h"

typedef struct {
    OutputBuffer* output_buffer;
} CabSession;

CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

#endif
