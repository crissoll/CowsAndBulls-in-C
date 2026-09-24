#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "cab_fh.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_session_cmd_tree.h"
#include "cab_tokens.h"
#include "cmd_spec.h"

void cab_fh__store_cmd_disable(CabSession* session, char* buffer) {
    if (session->commands_tree == NULL) {
        return;
    }
    const CabTokens* tokens_arr =
        session->commands_tree->disabled_commands_text;
    for (size_t i = 0; i < session->commands_tree->disabled_commands_text_size;
         i++) {
        const CabTokens* tokens = tokens_arr + i;

        for (size_t j = 0; j < tokens->token_count; j++) {
            buffer += sprintf(buffer, "%s ", tokens->tokens[j]);
        }
        buffer += sprintf(buffer, "\n");
    }
}

bool cab_fh__load_cmd_disable(CabSession* session, const char* buffer) {
    if (buffer == NULL) {
        return true;
    }
    CabTokens* token_arr = NULL;
    size_t* token_arr_size = NULL;
    if (session->commands_tree != NULL) {
        token_arr = session->commands_tree->disabled_commands_text;
        token_arr_size = &session->commands_tree->disabled_commands_text_size;
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
                token_arr = session->commands_tree->disabled_commands_text;
                token_arr_size =
                    &session->commands_tree->disabled_commands_text_size;
            }

            cab_tokens__populate(&token_arr[(*token_arr_size)++], line);
        }
        ptr += len;
        if (*ptr == '\r') {
            ptr++;
        }
        if (*ptr == '\n') {
            return true;
        }
    }
    return true;
}

bool cab_fh__validate_cmd_disable(CabSession* session) {
    if (session->commands_tree == NULL) {
        return true;
    }
    const CabTokens* token_arr = session->commands_tree->disabled_commands_text;
    size_t token_arr_size = session->commands_tree->disabled_commands_text_size;
    const CommandSpec* root = cab_session__get_cmd_tree_root(session);
    silence_messages(session);
    const char* disable_text = "disable";
    const CommandSpec* disable_spec =
        find_command_spec_in_tree(session, 1, &disable_text, root);
    for (size_t i = 0; i < token_arr_size; i++) {
        parse_command(session, disable_spec, (const char**)token_arr[i].tokens,
                      token_arr[i].token_count);
    }

    unsilence_messages(session);
    return true;
}

const CabFileHandler cab_fh_cmd_disable = {
    .name = "Disable",
    .load_function = cab_fh__load_cmd_disable,
    .store_function = cab_fh__store_cmd_disable,
    .validation_function = cab_fh__validate_cmd_disable,
};
