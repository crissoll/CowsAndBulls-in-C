#include <string.h>

#include "cab_io_consts.h"
#include "cmd_spec.h"
#include "cmd_tree.h"

#include "cab_output.h"


void print_help_text(const char* command_name) {
    const CommandSpec* candidate_spec = get_cmd_tree_root()->args;
    while (candidate_spec->name != NULL) {
        if (!(*candidate_spec->allowed)) {
            candidate_spec++;
            continue;
        }
        if (strcmp(candidate_spec->name, command_name) == 0) {
            message(OT_HELP, candidate_spec->help_text);
            return;
        }
        candidate_spec++;
    }
    message(OT_ALERT, "command not found!\n");
}


void print_help_text_from_tokens(size_t token_count, const char* tokens[]) {
    if (token_count > 1) {
        alert_too_many_arguments();
        return;
    }
    print_help_text(tokens[0]);
}

void print_whole_help_text() {
    const CommandSpec* candidate_spec = get_cmd_tree_root()->args;
    start_message(OT_HELP);
    while (candidate_spec->name != NULL) {
        if (!(*candidate_spec->allowed)) {
            continue;
        }
        output(candidate_spec->help_text);
        candidate_spec++;
    }
    end_message();
}
