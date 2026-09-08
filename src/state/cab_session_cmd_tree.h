#ifndef CAB_SESSION_CMD_TREE
#define CAB_SESSION_CMD_TREE

#include "cmd_spec.h"

typedef struct CabSession CabSession;

typedef struct {
    const CommandSpec* root;
    const CommandSpec** disabled_commands;
    size_t disabled_commands_current_size;
    size_t disabled_commands_allocated_size;
    CabCmdDisabledFlags* disabled_slots;
} CmdTree;

void cab_cmd_tree__free_content(CmdTree* tree);

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
