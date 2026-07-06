#include <stddef.h>

#include "cmd_spec.h"
#include "cmd_tree.h"

void disable_command(size_t token_count, const char* tokens[]) {
    _disable_command(token_count, tokens, get_cmd_tree_root());
}
