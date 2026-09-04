#include <string.h>

#include "cab_io_consts.h"
#include "cmd_spec.h"
#include "cmd_tree.h"

#include "cab_output.h"

void print_help_text(const char* command_name) {
    const CommandSpec* candidate_spec = get_cmd_tree_root()->args;
    while (!command_spec_is_end_spec(*candidate_spec)) {
        const bool found = strcmp(candidate_spec->name, command_name) == 0;

        if (!(*candidate_spec->allowed) || !found) {
            candidate_spec++;
            continue;
        }

        if (candidate_spec->help_text == NULL) {
            break;
        }

        message(OT_HELP, "%s", candidate_spec->help_text);
        return;
    }
    message(OT_ALERT, "command documentation not found!\n");
}


void print_help_text_from_tokens(size_t token_count, const char* tokens[]) {
    if (token_count > 1) {
        alert_too_many_arguments(token_count, tokens);
        return;
    }
    print_help_text(tokens[0]);
}


void print_whole_help_text(void) {
    const CommandSpec* cur_spec = get_cmd_tree_root()->args;
    start_message(OT_HELP);
    while (!command_spec_is_end_spec(*cur_spec)) {
        if ((*cur_spec->allowed) && cur_spec->help_text != NULL) {
            output("%s", cur_spec->help_text);
        }
        cur_spec++;
    }
    end_message();
}
