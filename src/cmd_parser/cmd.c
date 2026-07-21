#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

#include "cab_errors.h"

#include "cab_io_consts.h"
#include "cab_output.h"
#include "cmd_tree.h"

static char special_command_char = '\0';

void set_special_command_char(char value) {
    if (isalpha(value)) {
        extra_io_warning(
            "set_special_command_char: value must not be a letter");
    }
}

void set_special_command_char_from_size_t(size_t value) {

    set_special_command_char((char)value);
}


void parse(const char* tokens[], size_t token_count) {
    if (special_command_char == '\0') {
        parse_command(get_cmd_tree_root(), tokens, token_count);
        return;
    }

    if (tokens[0][0] == special_command_char) {
        tokens[0]++;
        if (!parse_args(get_cmd_tree_root(), tokens, token_count)) {
            message(OT_USER, "command not found!");
        }
        tokens[0]--;
        return;
    }
    get_cmd_tree_root()->default_handler(token_count, tokens);
}
