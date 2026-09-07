#include <stddef.h>

#include "cab_session.h"
#include "cmd_spec.h"


void disable_command_from_tree(CabSession* session, size_t token_count,
                               const char* tokens[]) {
    disable_command(session, token_count, tokens,
                    cab_session__get_cmd_tree_root(session));
}
