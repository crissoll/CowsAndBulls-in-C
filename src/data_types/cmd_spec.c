#include <string.h>

#include "cab_output.h"
#include "cmd_spec.h"


void alert_too_many_arguments() {
    output("too many arguments\n");
}

void alert_too_few_arguments() {
    output("too few arguments\n");
}

void parse_command(const CommandSpec* specifier, const char* tokens[],
                   size_t token_count);

bool parse_args(const CommandSpec* specifier, const char* tokens[],
                size_t token_count) {
    if (specifier->args == NULL) {
        return false;
    }

    const CommandSpec* candidate_arg = specifier->args;
    while (candidate_arg->name != NULL) {
        if (!(*candidate_arg->allowed)) {
            candidate_arg++;
            output("command is not allowed\n");
            return true;
        }
        if (strcmp(tokens[0], candidate_arg->name) == 0) {
            parse_command(candidate_arg, tokens + 1, token_count - 1);
            return true;
        }
        candidate_arg++;
    }
    return false;
}

void parse_command(const CommandSpec* specifier, const char* tokens[],
                   size_t token_count) {
    if (token_count == 0) {
        if (specifier->case_no_args == NULL) {
            alert_too_few_arguments();
            return;
        }
        specifier->case_no_args();
        return;
    }
    const bool args_correctly_parsed =
        parse_args(specifier, tokens, token_count);

    if (args_correctly_parsed) {
        return;
    }
    if (specifier->default_handler == NULL) {
        alert_too_many_arguments();
        return;
    }
    specifier->default_handler(token_count, tokens);
    return;
}


void _disable_command(size_t token_count, const char* tokens[],
                      const CommandSpec* base_spec) {
    const CommandSpec* candidate_spec = base_spec->args;
    while (candidate_spec->name != NULL) {
        if (strcmp(candidate_spec->name, tokens[0]) == 0) {
            if (token_count == 1) {
                *candidate_spec->allowed = false;
                return;
            }
            output("%s\n", candidate_spec->name);
            if (candidate_spec->args == NULL) {
                alert_too_many_arguments();
                return;
            }
            _disable_command(token_count - 1, tokens + 1, candidate_spec);
            return;
        }
        candidate_spec++;
    }
    output("command not found!\n");
    return;
}
