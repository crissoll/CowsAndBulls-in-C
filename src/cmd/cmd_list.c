#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_settings_override.h"
#include "cmd_list.h"
#include "word.h"
#include "word_set_filter.h"

static bool check_pattern(const char pattern[MAX_PRACTICAL_WORD_LEN + 1],
                          size_t word_len) {
    const size_t pattern_len = strlen(pattern);

    if (pattern_len == 1) {
        return (pattern[0] >= 'a' && pattern[0] <= 'z') ||
               pattern[0] == UNDEFINED_LETTER;
    }

    if (pattern_len != word_len) {
        return false;
    }

    if (pattern[word_len] != '\0') {
        return false;
    }

    for (size_t k = 0; k < word_len; k++) {
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

void load_filter_from_history(CabSession* session, size_t token_count,
                              const char* tokens[]) {
    if (token_count != 1) {
        message(session, OT_INPUT_ERROR, "expected one index argument\n");
        return;
    }

    int index;
    if (sscanf(tokens[0], "%d", &index) != 1) {
        message(session, OT_INPUT_ERROR, "index must be a number\n");
        return;
    }

    const size_t history_size = cab_session__get_filter_history_size(session);
    if (history_size == 0) {
        message(session, OT_INPUT_ERROR, "no history available\n");
        return;
    }

    if (index < 0) {
        index = (int)history_size + index + 1;
    }

    if (index <= 0) {
        message(session, OT_INPUT_ERROR, "index must be > 0\n");
        return;
    }
    if ((size_t)index > history_size) {
        message(session, OT_INPUT_ERROR, "index too high!\n");
        return;
    }

    cab_session__word_filter_revert_to(session, (size_t)(index - 1));

    message(session, OT_USER, "correctly reverted to step number %d\n", index);
    const size_t word_count =
        cab_session__get_last_word_filter(session).word_count;
    message(session, OT_WORD_COUNT, "[%zu words]\n", word_count);
}

void cmd__list_remove_letters(CabSession* session, size_t token_count,
                              const char* tokens[]) {
    const size_t word_len =
        cab_session__get_setting(*session, STG_Internal_WordLen);
    for (size_t i = 0; i < token_count; i++) {
        if (!check_pattern(tokens[i], word_len)) {
            message(session, OT_INPUT_ERROR, "invalid pattern!\n");
            return;
        }
    }

    WordSetFilter filter = cab_session__get_last_word_filter(session).filter;
    for (size_t i = 0; i < token_count; i++) {
        filter__apply_pattern(&filter, tokens[i], REMOVE);
    }

    const size_t word_count =
        cab_session__compute_filter_word_count(session, &filter);
    cab_session__word_filter_add_entry(
        session, (ListHistoryEntry){.filter = filter, .word_count = word_count});
    message(session, OT_WORD_COUNT, "[%zu words]\n", word_count);
}

void cmd__list_intersect_letters(CabSession* session, size_t token_count,
                                 const char* tokens[]) {
    const size_t word_len =
        cab_session__get_setting(*session, STG_Internal_WordLen);
    for (size_t i = 0; i < token_count; i++) {
        if (!check_pattern(tokens[i], word_len)) {
            message(session, OT_INPUT_ERROR, "invalid pattern!\n");
            return;
        }
    }

    WordSetFilter filter = cab_session__get_last_word_filter(session).filter;
    for (size_t i = 0; i < token_count; i++) {
        filter__apply_pattern(&filter, tokens[i], INTERSECT);
    }

    const size_t word_count =
        cab_session__compute_filter_word_count(session, &filter);
    cab_session__word_filter_add_entry(
        session, (ListHistoryEntry){.filter = filter, .word_count = word_count});
    message(session, OT_WORD_COUNT, "[%zu words]\n", word_count);
}

void setup_list_from_pattern(CabSession* session, size_t token_count,
                             const char* tokens[]) {
    if (token_count > 1) {
        message(session, OT_INPUT_ERROR,
                "list can only be initialized with a single pattern\n");
        return;
    }
    const size_t word_len =
        cab_session__get_setting(*session, STG_Internal_WordLen);
    if (!check_pattern(tokens[0], word_len)) {
        message(session, OT_INPUT_ERROR, "invalid pattern!\n");
        return;
    }

    WordSetFilter filter;
    filter__init(&filter);
    if (!is_undefined_pattern(tokens[0])) {
        filter__apply_pattern(&filter, tokens[0], INTERSECT);
    }

    const size_t word_count =
        cab_session__compute_filter_word_count(session, &filter);
    cab_session__word_filter_add_entry(
        session, (ListHistoryEntry){.filter = filter, .word_count = word_count});
    message(session, OT_WORD_COUNT, "[%zu words]\n", word_count);
}
