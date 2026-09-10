#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"
#include "cmd_spec.h"


void parse(CabSession* session, const char* tokens[], size_t token_count) {
    if (cab_session__get_setting(*session, STG_Internal_CommandPrefixChar) !=
        '\0') {
        if (tokens[0][0] != (char)cab_session__get_setting(
                                *session, STG_Internal_CommandPrefixChar)) {
            cab_session__get_cmd_tree_root(session)->default_handler(
                session, token_count, tokens);
            return;
        }
        tokens[0]++;
    }
    parse_command(session, cab_session__get_cmd_tree_root(session), tokens,
                  token_count);
}
