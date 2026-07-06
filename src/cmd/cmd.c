#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cmd_tree.h"

void parse(const char* tokens[], size_t token_count) {
    parse_command(get_cmd_tree_root(), tokens, token_count);
}
