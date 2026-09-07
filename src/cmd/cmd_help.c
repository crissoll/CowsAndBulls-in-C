#include <stdbool.h>
#include <string.h>

#include "cab_io_consts.h"
#include "cab_output.h"
#include "cmd_spec.h"


void print_help_text(CabSession* session, const char* command_name) {
    const CommandSpec* candidate_spec =
        cab_session__get_cmd_tree_root(session)->args;
    while (!command_spec_is_end_spec(*candidate_spec)) {
        const bool found =
            command_spec_name_match(*candidate_spec, command_name);

        if (cab_session__is_command_allowed(*session, candidate_spec) ==
                false ||
            !found) {
            candidate_spec++;
            continue;
        }

        if (candidate_spec->help_text == NULL) {
            break;
        }

        message(session, OT_HELP, "%s", candidate_spec->help_text);
        return;
    }
    message(session, OT_ALERT, "command documentation not found!\n");
}


void print_help_text_from_tokens(CabSession* session, size_t token_count,
                                 const char* tokens[]) {
    if (token_count > 1) {
        alert_too_many_arguments(session, token_count, tokens);
        return;
    }
    print_help_text(session, tokens[0]);
}


void print_whole_help_text(CabSession* session) {
    const CommandSpec* cur_spec = cab_session__get_cmd_tree_root(session)->args;
    start_message(session, OT_HELP);
    while (!command_spec_is_end_spec(*cur_spec)) {
        if ((cab_session__is_command_allowed(*session, cur_spec)) &&
            cur_spec->help_text != NULL) {
            output(session, "%s", cur_spec->help_text);
        }
        cur_spec++;
    }
    end_message(session);
}
