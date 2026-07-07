#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_io_consts.h"
#include "word_set_filter.h"

#include "cab_help_filter.h"

#include "cab_output.h"

#include "cmd_list.h"

static bool check_pattern(const char pattern[LETTERS_IN_WORD + 1]) {
    const size_t pattern_len = strlen(pattern);

    if (pattern_len == 1) {
        return (pattern[0] >= 'a' && pattern[0] <= 'z') ||
               pattern[0] == UNDEFINED_LETTER;
    }

    if (pattern_len != LETTERS_IN_WORD) {
        return false;
    }

    if (pattern[LETTERS_IN_WORD] != '\0') {
        return false;
    }

    for (size_t k = 0; k < LETTERS_IN_WORD; k++) {
        const char c = pattern[k];
        if (c == '\0' || ((c < 'a' || c > 'z') && c != UNDEFINED_LETTER)) {
            return false;
        }
    }
    return true;
}

static bool is_undefined_pattern(const char* pattern) {
    for (size_t i = 0; i < strlen(pattern); i++) {
        if (pattern[i] != UNDEFINED_LETTER) {
            return false;
        }
    }
    return true;
}

static void cmd_list__set_pattern(const char pattern[LETTERS_IN_WORD + 1]) {
    WordSetFilter* help_filter = get_current_help_filter();
    filter__init(help_filter);
    filter__apply_pattern(help_filter, pattern, INTERSECT);
}

void load_filter_from_history(size_t token_count, const char* tokens[]) {
    if (token_count != 1) {
        message(OT_INPUT_ERROR, "expected one index argument\n");
        return;
    }

    int index;

    if (sscanf(tokens[0], "%d", &index) != 1) {
        message(OT_INPUT_ERROR, "index must be a number\n");
        return;
    }
    if (index < 0) {
        index = ((int)get_filter_history_size()) + index;
        if (index <= 0) {
            message(OT_INPUT_ERROR, "relative index too low\n");
            return;
        }
    }
    index--;
    if (index < 0) {
        message(OT_INPUT_ERROR, "index must be > 0\n");
        return;
    }
    if ((size_t)index >= get_filter_history_size()) {
        message(OT_INPUT_ERROR, "index too high!\n");
        return;
    }

    revert_filter_to_history_step(index);

    message(OT_USER, "correctly reverted to step number %d\n", index + 1);

    add_current_filter_to_history();
    const size_t word_count = get_current_help_filter_word_count();
    message(OT_WORD_COUNT, "[%zu words]\n", word_count);
}

bool cmd__list_parse_all_patterns(size_t patterns_count, const char* patterns[],
                                  FilterMode mode) {
    WordSetFilter* help_filter = get_current_help_filter();

    for (size_t arg_idx = 0; arg_idx < patterns_count; arg_idx++) {
        if (!check_pattern(patterns[arg_idx])) {
            return false;
        }

        filter__apply_pattern(help_filter, patterns[arg_idx], mode);
    }
    return true;
}

void cmd__list_remove_letters(size_t token_count, const char* tokens[]) {
    cmd__list_parse_all_patterns(token_count, tokens, REMOVE);

    add_current_filter_to_history();
    const size_t word_count = get_current_help_filter_word_count();
    message(OT_WORD_COUNT, "[%zu words]\n", word_count);
}

void cmd__list_intersect_letters(size_t token_count, const char* tokens[]) {
    cmd__list_parse_all_patterns(token_count, tokens, INTERSECT);

    add_current_filter_to_history();
    const size_t word_count = get_current_help_filter_word_count();
    message(OT_WORD_COUNT, "[%zu words]\n", word_count);
}

void setup_list_from_pattern(size_t token_count, const char* tokens[]) {
    if (token_count > 1) {
        message(OT_INPUT_ERROR,
                "list can only be initialized with a single pattern\n");
    }
    WordSetFilter* help_filter = get_current_help_filter();

    if (!check_pattern(tokens[0])) {
        message(OT_INPUT_ERROR, "invalid pattern!\n");
        return;
    }

    if (is_undefined_pattern(tokens[0])) {
        filter__init(help_filter);
    } else {
        cmd_list__set_pattern(tokens[0]);
    }

    add_current_filter_to_history();
    const size_t word_count = get_current_help_filter_word_count();
    message(OT_WORD_COUNT, "[%zu words]\n", word_count);
}
