#include <stdbool.h>
#include <stddef.h>

#include "cab_settings_api.h"
#include "cmd_spec.h"
#include "cmd_tree.h"

void parse(const char* tokens[], size_t token_count) {
    if (cab_get_setting(STG_Internal_CommandPrefixChar) != '\0') {
        if (tokens[0][0] !=
            (char)cab_get_setting(STG_Internal_CommandPrefixChar)) {
            get_cmd_tree_root()->default_handler(token_count, tokens);
            return;
        }
        tokens[0]++;
    }
    parse_command(get_cmd_tree_root(), tokens, token_count);
}
