
#include <stdbool.h>
#include "cab_attempts_manager.h"

#include "cab_help_filter.h"
#include "cmd_attempts.h"
#include "cmd_disable.h"
#include "cmd_docs.h"
#include "cmd_help.h"
#include "cmd_list.h"
#include "cmd_spec.h"
#include "cmd_surrender.h"
#include "cmd_try_word.h"


static const CommandSpec command_specs[] = {
    {
        .name = "disable",
        .case_no_args = alert_too_few_arguments,
        .default_handler = disable_command,
        .args =
            (const CommandSpec[]){
                {
                    .name = "--reset",
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = disable_command_reset,
                    .args = NULL,
                },
                {
                    .name = "--zero-args",
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = disable_command_no_args,
                    .args = NULL,
                },
                {
                    .name = "--args",
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = disable_command_args,
                    .args = NULL,
                },
                {
                    .name = "--default",
                    .case_no_args = alert_too_few_arguments,
                    .default_handler = disable_command_default_handler,
                    .args = NULL,
                },
                END_SPEC,
            },
    },
    {
        .name = "help",
        .help_text = HELP_CMD_HELP,
        .case_no_args = print_whole_help_text,
        .default_handler = print_help_text_from_tokens,
        .args = NULL,
    },
    {
        .name = "surrender",
        .help_text = HELP_CMD_SURRENDER,
        .case_no_args = cmd__surrender,
        .default_handler = alert_too_many_arguments,
        .args = NULL,

    },
    {
        .name = "attempts",
        .help_text = HELP_CMD_ATTEMPTS,
        .case_no_args = cab_session__print_attempts,
        .default_handler = compare_attempts_to_first_token,
        .args = NULL,
    },
    {
        .name = "list",
        .help_text = HELP_CMD_LIST,
        .case_no_args = print_current_filter,
        .default_handler = setup_list_from_pattern,
        .args =
            (const CommandSpec[]){
                {
                    .name = "-p",
                    .case_no_args = print_filtered_word_list,
                    .default_handler = NULL,
                    .args = NULL,
                },
                {
                    .name = "-h",
                    .case_no_args = print_filter_history,
                    .default_handler = load_filter_from_history,
                    .args = NULL,
                },
                {
                    .name = "-r",
                    .default_handler = cmd__list_remove_letters,
                    .args = NULL,
                },
                {
                    .name = "-i",
                    .default_handler = cmd__list_intersect_letters,
                    .args = NULL,
                },
                END_SPEC,
            },
    },
    END_SPEC,
};


static const CommandSpec* const ROOT = &(const CommandSpec){
    .case_no_args = NULL,
    .default_handler = cmd__try_word_from_tokens,
    .args = command_specs,
};

const CommandSpec* get_default_cmd_tree_root(void) {
    return ROOT;
}
