#include <string.h>

#include "cmd_spec.h"
#include "cmd_tree.h"

#include "cab_output.h"

void print_help_text_from_args(size_t token_count, const char* tokens[]) {
    if (token_count > 1) {
        alert_too_many_arguments();
        return;
    }
    const CommandSpec* candidate_spec = get_cmd_tree_root()->args;
    while (candidate_spec->name != NULL) {
        if (!(*candidate_spec->allowed)) {
            candidate_spec++;
            continue;
        }
        if (strcmp(candidate_spec->name, tokens[0]) == 0) {
            output(candidate_spec->help_text);
            return;
        }
        candidate_spec++;
    }
    output("command not found!\n");
}

void print_whole_help_text() {
    const CommandSpec* candidate_spec = get_cmd_tree_root()->args;
    while (candidate_spec->name != NULL) {
        if (!(*candidate_spec->allowed)) {
            continue;
        }
        output(candidate_spec->help_text);
        candidate_spec++;
    }
}
