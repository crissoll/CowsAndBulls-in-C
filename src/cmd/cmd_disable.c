#include <stddef.h>

#include "cmd_spec.h"
#include "cmd_tree.h"

void disable_command_from_tree(CabSession* session, size_t token_count,
                               const char* tokens[]) {
    disable_command(session, token_count, tokens, get_cmd_tree_root());
}
