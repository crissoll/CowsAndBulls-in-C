#include <stddef.h>

#include "cab_output.h"
#include "cab_session.h"
#include "cmd_spec.h"


void command_spec__set_disable_flags(CabSession* session, size_t token_count,
                                     const char* tokens[],
                                     CabCmdDisabledFlags flags) {
    const CommandSpec* spec = find_command_spec_in_tree(
        session, token_count, tokens, cab_session__get_cmd_tree_root(session));

    if (spec == NULL) {
        message(session, OT_ALERT, "command not found!\n");
        return;
    }
    const CabCmdDisabledFlags old_flags =
        cab_session__get_command_spec_disable_flags(*session, spec);
    cab_session__set_command_spec_disable_flags(session, spec, flags);
    const char* flag_text;
    switch (flags) {
        case CMD_DISABLE_NOTHING:
            if (old_flags != CMD_DISABLE_NOTHING) {
                message(session, OT_USER, "%s has been enabled\n", spec->name);
            }
            return;
        case CMD_DISABLE_ALL:
            flag_text = " has";
            break;
        case CMD_DISABLE_ARGS:
            flag_text = "'s args have";
            break;
        case CMD_DISABLE_DEF_HANDLER:
            flag_text = "'s default handler has";
            break;
        case CMD_DISABLE_NO_ARGS:
            flag_text = "'s no argument case has";
            break;
    }
    message(session, OT_USER, "%s%s been disabled\n", spec->name, flag_text);
}


void disable_command(CabSession* session, size_t token_count,
                     const char* tokens[]) {
    command_spec__set_disable_flags(session, token_count, tokens,
                                    CMD_DISABLE_ALL);
}


void disable_command_args(CabSession* session, size_t token_count,
                          const char* tokens[]) {
    command_spec__set_disable_flags(session, token_count, tokens,
                                    CMD_DISABLE_ARGS);
}


void disable_command_default_handler(CabSession* session, size_t token_count,
                                     const char* tokens[]) {
    command_spec__set_disable_flags(session, token_count, tokens,
                                    CMD_DISABLE_DEF_HANDLER);
}

void disable_command_no_args(CabSession* session, size_t token_count,
                             const char* tokens[]) {
    command_spec__set_disable_flags(session, token_count, tokens,
                                    CMD_DISABLE_NO_ARGS);
}


void disable_command_reset(CabSession* session, size_t token_count,
                           const char* tokens[]) {
    command_spec__set_disable_flags(session, token_count, tokens,
                                    CMD_DISABLE_NOTHING);
}
