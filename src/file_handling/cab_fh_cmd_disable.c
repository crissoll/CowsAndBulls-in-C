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
    const CabTokens* tokens_arr =
        session->commands_tree->disabled_commands_tokens.array;
    for (size_t i = 0;
         i < session->commands_tree->disabled_commands_tokens.size; i++) {
        const CabTokens* tokens = tokens_arr + i;

        for (size_t j = 0; j < tokens->token_count; j++) {
            buffer += sprintf(buffer, "%s ", tokens->tokens[j]);
        }
        if (tokens->token_count > 0) {
            buffer += sprintf(buffer, "\n");
        }
    }
}

bool cab_fh__load_cmd_disable(CabSession* session, const char* buffer) {
    if (buffer == NULL) {
        return true;
    }
    CabTokensArray* token_array = NULL;
    if (session->commands_tree != NULL) {
        token_array = &session->commands_tree->disabled_commands_tokens;
    }

    const char* ptr = buffer;
    while (*ptr != '\0') {
        size_t len = strcspn(ptr, "\r\n");
        if (len > 0) {
            char line[256];
            if (len >= sizeof(line)) {
                len = sizeof(line) - 1;
            }
            memcpy(line, ptr, len);
            line[len] = '\0';

            if (session->commands_tree == NULL) {
                session->commands_tree =
                    calloc(1, sizeof(*session->commands_tree));
                token_array = &session->commands_tree->disabled_commands_tokens;
            }
            CabTokens new_tokens = {0};
            cab_tokens__populate(&new_tokens, line);
            cab_tokens_array__add_element(token_array, new_tokens);
        }
        ptr += len;
        if (*ptr == '\r' || *ptr == '\n') {
            ptr++;
        }
        if (*ptr == '\n') {
            ptr++;
        }
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
