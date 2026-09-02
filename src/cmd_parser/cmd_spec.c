#include <stdbool.h>
#include <string.h>

#include "attempts.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cmd_spec.h"


void alert_too_many_arguments() {
    message(OT_INPUT_ERROR, "too many arguments\n");
}

void alert_too_few_arguments() {
    message(OT_INPUT_ERROR, "too few arguments\n");
}

const CommandSpec* command_spec_find_arg(const CommandSpec* parent,
                                         const char* searched_name) {
    if (parent == NULL || searched_name == NULL) {
        message(OT_WARNING,
                "command_spec_find_arg: NULL arguments not accepted");
        return NULL;
    }
    const CommandSpec* candidate_arg = parent->args;
    if (candidate_arg == NULL) {
        return NULL;
    }
    while (candidate_arg->name != NULL) {
        if (strcmp(searched_name, candidate_arg->name) == 0) {
            return candidate_arg;
        }
        candidate_arg++;
    }
    return NULL;
}

void parse_command(const CommandSpec* specifier, const char* tokens[],
                   size_t token_count) {
    if (specifier->allowed == false) {
        return;
    }

    if (token_count == 0) {
        if (specifier->case_no_args == NULL) {
            alert_too_few_arguments();
            return;
        }
        specifier->case_no_args();
        return;
    }
    const CommandSpec* argument = command_spec_find_arg(specifier, tokens[0]);

    if (argument != NULL) {
        parse_command(argument, tokens + 1, token_count - 1);
        return;
    }

    if (specifier->default_handler == NULL) {
        alert_too_many_arguments();
        return;
    }
    specifier->default_handler(token_count, tokens);
    return;
}


void disable_command(size_t token_count, const char* tokens[],
                     const CommandSpec* base_spec) {
    const CommandSpec* candidate_spec = base_spec->args;
    while (candidate_spec->name != NULL) {
        if (strcmp(candidate_spec->name, tokens[0]) == 0) {
            if (token_count == 1) {
                *candidate_spec->allowed = false;
                message(OT_USER, "%s has been disabled\n",
                        candidate_spec->name);
                return;
            }

            if (candidate_spec->args == NULL) {
                alert_too_many_arguments();
                return;
            }
            disable_command(token_count - 1, tokens + 1, candidate_spec);
            return;
        }
        candidate_spec++;
    }
    message(OT_ALERT, "command not found!\n");
    return;
}
