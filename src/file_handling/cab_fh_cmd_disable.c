#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_fh.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_session_cmd_tree.h"
#include "cab_session_parsing.h"
#include "cab_tokens.h"
#include "cmd_spec.h"


void cab_fh__store_cmd_disable(CabSession* session, char* buffer) {
    if (session->commands_tree == NULL) {
        return;
    }
    const CabTokensArray* tokens_arr =
        &session->commands_tree->disabled_commands_tokens;
    cab_tokens_array__store(tokens_arr, buffer);
}

bool cab_fh__load_cmd_disable(CabSession* session, const char* buffer) {
    if (buffer == NULL) {
        return true;
    }

    if (session->commands_tree == NULL) {
        session->commands_tree = calloc(1, sizeof(*session->commands_tree));
    }
    CabTokensArray* token_array =
        &session->commands_tree->disabled_commands_tokens;

    cab_tokens_array__load(token_array, buffer);


    if (session->commands_tree->disabled_commands_tokens.size == 0) {
        cab_cmd_tree__free_content(session->commands_tree);
        session->commands_tree = NULL;
    }
    return true;
}

bool cab_fh__validate_cmd_disable(CabSession* session) {
    if (session->commands_tree == NULL) {
        return true;
    }
    const CommandSpec* root = cab_session__get_cmd_tree_root(session);

    CabTokensArray array = session->commands_tree->disabled_commands_tokens;
    session->commands_tree->disabled_commands_tokens = (CabTokensArray){0};

    silence_messages(session);
    const char* disable_text = "disable";
    const CommandSpec* disable_spec =
        find_command_spec_in_tree(session, 1, &disable_text, root);
    cab_session__parse_tokens_array(session, array, disable_spec);
    unsilence_messages(session);
    session->commands_tree->disabled_commands_tokens = array;
    return true;
}

const CabFileHandler cab_fh_cmd_disable = {
    .name = "Disable",
    .load_function = cab_fh__load_cmd_disable,
    .store_function = cab_fh__store_cmd_disable,
    .validation_function = cab_fh__validate_cmd_disable,
};
