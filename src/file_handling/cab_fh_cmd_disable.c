#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_fh.h"
#include "cab_session.h"
#include "cab_session_cmd_tree.h"
#include "cab_tokens.h"

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
    CabTokens* token_arr = session->commands_tree->disabled_commands_text;
    size_t* token_arr_size =
        &session->commands_tree->disabled_commands_text_size;
    *token_arr_size = 0;
    if (buffer == NULL) {
        return true;
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
    return true;
}

const CabFileHandler cab_fh_cmd_disable = {
    .name = "Disable",
    .load_function = cab_fh__load_cmd_disable,
    .store_function = cab_fh__store_cmd_disable,
};
