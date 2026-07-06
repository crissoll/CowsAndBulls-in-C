

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cab_attempts_manager.h"


#include "cab_help_filter.h"
#include "cab_output.h"
#include "cmd_attempts.h"
#include "cmd_docs.h"
#include "cmd_list.h"
#include "cmd_spec.h"
#include "cmd_try_word.h"

void print_whole_help_text();

void print_help_text_from_args(size_t token_count, const char* tokens[]);

void disable_command(size_t token_count, const char* tokens[]);

const CommandSpec command_specs[] = {
    {
        .name = "disable",
        .help_text = "disable_TMP\n",
        .case_no_args = alert_too_few_arguments,
        .allowed = &(bool){true},
        .default_handler = disable_command,
        .args = NULL,
    },
    {
        .name = "help",
        .help_text = HELP_CMD_HELP,
        .allowed = &(bool){true},
        .case_no_args = print_whole_help_text,
        .default_handler = print_help_text_from_args,
        .args = NULL,
    },
    {
        .name = "attempts",
        .help_text = HELP_CMD_ATTEMPTS,
        .allowed = &(bool){true},
        .case_no_args = print_attempts,
        .default_handler = compare_attempts_to_first_arg,
        .args = NULL,
    },
    {
        .name = "list",
        .help_text = HELP_CMD_LIST,
        .allowed = &(bool){true},
        .case_no_args = print_current_filter,
        .default_handler = setup_list_from_pattern,
        .args =
            (const CommandSpec[]){
                {
                    .name = "-p",
                    .allowed = &(bool){true},
                    .case_no_args = print_filtered_word_list,
                    .default_handler = NULL,
                    .args = NULL,
                },
                {
                    .name = "-h",
                    .allowed = &(bool){true},
                    .case_no_args = print_filter_history,
                    .default_handler = load_filter_from_history,
                    .args = NULL,
                },
                {
                    .name = "-r",
                    .allowed = &(bool){true},
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = cmd__list_remove_letters,
                    .args = NULL,
                },
                {
                    .name = "-i",
                    .allowed = &(bool){true},
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = cmd__list_intersect_letters,
                    .args = NULL,
                },
                END_SPEC,
            },
    },
    END_SPEC,
};

const CommandSpec* ROOT =
    &(CommandSpec){.case_no_args = NULL,
                   .default_handler = cmd__try_word_from_args,
                   .args = command_specs};


void print_help_text_from_args(size_t token_count, const char* tokens[]) {
    if (token_count > 1) {
        alert_too_many_arguments();
        return;
    }
    const CommandSpec* candidate_spec = command_specs;
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
    return;
}

void disable_command(size_t token_count, const char* tokens[]) {
    _disable_command(token_count, tokens, ROOT);
}

void print_whole_help_text() {
    const CommandSpec* candidate_spec = command_specs;
    while (candidate_spec->name != NULL) {
        if (!(*candidate_spec->allowed)) {
            continue;
        }
        output(candidate_spec->help_text);
        candidate_spec++;
    }
    return;
}

void parse(const char* tokens[], size_t token_count) {
    parse_command(ROOT, tokens, token_count);
}
