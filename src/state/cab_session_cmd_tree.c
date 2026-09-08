#include <malloc.h>


#include "cab_session.h"
#include "cmd_tree.h"


void cab_session_set_cmd_root(CabSession* session,
                              const CommandSpec* specifier) {
    if (session->commands_tree == NULL) {
        session->commands_tree = calloc(1, sizeof(*session->commands_tree));
    }
    session->commands_tree->root = specifier;
}

static int cab_session__find_disabled_command(CabSession session,
                                              const CommandSpec* specifier) {
    if (session.commands_tree == NULL ||
        session.commands_tree->disabled_commands == NULL) {
        return -1;
    }
    CmdTree command_tree = *session.commands_tree;
    for (size_t i = 0; i < command_tree.disabled_commands_current_size; i++) {
        if (specifier == command_tree.disabled_commands[i]) {
            return (int)i;
        }
    }
    return -1;
}

bool cab_session__is_command_allowed(CabSession session,
                                     const CommandSpec* specifier) {
    CabCmdDisabledFlags flags =
        cab_session__get_command_spec_disable_flags(session, specifier);
    return flags != CMD_DISABLE_ALL;
}

CabCmdDisabledFlags cab_session__get_command_spec_disable_flags(
    CabSession session, const CommandSpec* specifier) {
    int index = cab_session__find_disabled_command(session, specifier);
    if (index < 0) {
        return CMD_DISABLE_NOTHING;
    }
    return session.commands_tree->disabled_slots[index];
}


void cab_session__set_command_spec_disable_flags(CabSession* session,
                                                 const CommandSpec* specifier,
                                                 CabCmdDisabledFlags flags) {

    if (session->commands_tree == NULL) {
        session->commands_tree = calloc(1, sizeof(*session->commands_tree));
    }
    CmdTree* tree = session->commands_tree;
    const int index = cab_session__find_disabled_command(*session, specifier);
    if (index != -1) {
        if (flags == CMD_DISABLE_NOTHING) {
            tree->disabled_slots[index] = CMD_DISABLE_NOTHING;
            return;
        }
        tree->disabled_slots[index] |= flags;
        return;
    }

    if (tree->disabled_commands == NULL) {
        tree->disabled_commands_allocated_size = 4;
        tree->disabled_commands =
            malloc(sizeof(*tree->disabled_commands) *
                   tree->disabled_commands_allocated_size);
        tree->disabled_slots = malloc(sizeof(*tree->disabled_slots) *
                                      tree->disabled_commands_allocated_size);
        tree->disabled_commands_current_size = 0;
    } else if (tree->disabled_commands_current_size >=
               tree->disabled_commands_allocated_size) {
        tree->disabled_commands_allocated_size *= 3;
        tree->disabled_commands_allocated_size /= 2;
        tree->disabled_commands =
            realloc(tree->disabled_commands,
                    sizeof(*tree->disabled_commands) *
                        tree->disabled_commands_allocated_size);
        tree->disabled_slots = realloc(
            tree->disabled_slots, sizeof(*tree->disabled_slots) *
                                      tree->disabled_commands_allocated_size);
    }
    tree->disabled_commands[tree->disabled_commands_current_size] = specifier;
    tree->disabled_slots[tree->disabled_commands_current_size] = flags;
    tree->disabled_commands_current_size++;
}

const CommandSpec* cab_session__get_cmd_tree_root(CabSession* session) {
    return (session->commands_tree == NULL ||
            session->commands_tree->root == NULL)
               ? get_default_cmd_tree_root()
               : session->commands_tree->root;
}


void cab_cmd_tree__free_content(CmdTree* tree) {
    if (tree->disabled_commands != NULL) {
        free(tree->disabled_commands);
    }
    if (tree->disabled_slots != NULL) {
        free(tree->disabled_slots);
    }
}
