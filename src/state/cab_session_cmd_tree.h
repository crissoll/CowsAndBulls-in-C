#ifndef CAB_SESSION_CMD_TREE
#define CAB_SESSION_CMD_TREE

#include "cab_session_fwd.h"
#include "cab_tokens.h"
#include "cmd_spec.h"


#define MAX_DISABLE_COUNT 64

typedef struct {
    const CommandSpec* root;
    const CommandSpec** disabled_commands;
    size_t disabled_commands_current_size;
    size_t disabled_commands_allocated_size;
    CabCmdDisabledFlags* disabled_slots;
    CabTokensArray disabled_commands_tokens;
} CmdTree;

void cab_cmd_tree__free_content(CmdTree* tree);

void cab_session__set_command_spec_disable_flags(CabSession* session,
                                                 const CommandSpec* specifier,
                                                 CabCmdDisabledFlags flags);


bool cab_session__is_command_allowed(const CabSession* session,
                                     const CommandSpec* specifier);

const CommandSpec* cab_session__get_cmd_tree_root(CabSession* session);

void cab_session_set_cmd_root(CabSession* session,
                              const CommandSpec* specifier);
CabCmdDisabledFlags cab_session__get_command_spec_disable_flags(
    const CabSession* session, const CommandSpec* specifier);

void cab_cmd_tree__add_disabled_command_text(CmdTree* tree,
                                             const CabTokens* tokens);


#endif
