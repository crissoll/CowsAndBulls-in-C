#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cmd_spec.h"


void alert_too_many_arguments(CabSession* session, size_t token_count,
                              const char* tokens[]) {
    (void)token_count;
    (void)tokens;
    message(session, OT_INPUT_ERROR, "too many arguments\n");
}

void alert_too_few_arguments(CabSession* session) {
    message(session, OT_INPUT_ERROR, "too few arguments\n");
}

bool command_spec_is_end_spec(CommandSpec spec) {
    return spec.name == NULL;
}

bool command_spec_name_match(CommandSpec spec, const char* searched_name) {
    return strcmp(searched_name, spec.name) == 0;
}

const CommandSpec* command_spec_find_arg(const CommandSpec* parent,
                                         const char* searched_name) {
    if (parent == NULL || searched_name == NULL) {
        extra_io_warning(
            "command_spec_find_arg: NULL arguments not accepted\n");
        return NULL;
    }
    const CommandSpec* candidate_arg = parent->args;
    if (candidate_arg == NULL) {
        return NULL;
    }
    while (!command_spec_is_end_spec(*candidate_arg)) {
        if (command_spec_name_match(*candidate_arg, searched_name)) {
            return candidate_arg;
        }
        candidate_arg++;
    }
    return NULL;
}


void parse_command(CabSession* session, const CommandSpec* specifier,
                   const char* tokens[], size_t token_count) {
    if (cab_session__is_command_allowed(*session, specifier) == false) {
        return;
    }

    if (token_count == 0) {
        if (specifier->case_no_args == NULL) {
            alert_too_few_arguments(session);
            return;
        }
        specifier->case_no_args(session);
        return;
    }
    const CommandSpec* argument = command_spec_find_arg(specifier, tokens[0]);

    if (argument != NULL) {
        parse_command(session, argument, tokens + 1, token_count - 1);
        return;
    }

    if (specifier->default_handler == NULL) {
        alert_too_many_arguments(session, token_count, tokens);
        return;
    }
    specifier->default_handler(session, token_count, tokens);
    return;
}


void disable_command(CabSession* session, size_t token_count,
                     const char* tokens[], const CommandSpec* base_spec) {
    const CommandSpec* candidate_spec = base_spec->args;
    while (!command_spec_is_end_spec(*candidate_spec)) {
        if (strcmp(candidate_spec->name, tokens[0]) == 0) {
            if (token_count == 1) {
                cab_session__disable_command(session, base_spec);
                message(session, OT_USER, "%s has been disabled\n",
                        candidate_spec->name);
                return;
            }

            if (candidate_spec->args == NULL) {
                alert_too_many_arguments(session, token_count, tokens);
                return;
            }
            disable_command(session, token_count - 1, tokens + 1,
                            candidate_spec);
            return;
        }
        candidate_spec++;
    }
    message(session, OT_ALERT, "command not found!\n");
    return;
}
