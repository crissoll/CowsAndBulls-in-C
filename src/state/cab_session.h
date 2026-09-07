#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include <stddef.h>
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cmd_spec.h"

typedef struct {
    const CommandSpec* root;
    const CommandSpec** disabled_commands;
    size_t disabled_commands_current_size;
    size_t disabled_commands_allocated_size;
    CabCmdDisabledFlags* disabled_slots;
} CmdTree;

typedef struct CabSession {
    OutputBuffer* output_buffer;
    CAB_IOBuffer* input_buffer;
    CmdTree* commands_tree;
} CabSession;

CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

void cab_session__set_command_spec_disable_flags(CabSession* session,
                                                 const CommandSpec* specifier,
                                                 CabCmdDisabledFlags flags);


bool cab_session__is_command_allowed(CabSession session,
                                     const CommandSpec* specifier);

const CommandSpec* cab_session__get_cmd_tree_root(CabSession* session);

void cab_session_set_cmd_root(CabSession* session,
                              const CommandSpec* specifier);
CabCmdDisabledFlags cab_session__get_command_spec_disable_flags(
    CabSession session, const CommandSpec* specifier);

#endif
