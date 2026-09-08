#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include <stddef.h>
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_session_cmd_tree.h"
#include "cab_settings_override.h"


typedef struct CabSession {
    OutputBuffer* output_buffer;
    CAB_IOBuffer* input_buffer;
    CmdTree* commands_tree;
    CabSettingsOverride* settings_override;
} CabSession;

CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

#endif
