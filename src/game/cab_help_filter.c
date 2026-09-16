#include <stdlib.h>

#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_settings_override.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word_set.h"
#include "word_set_filter.h"

void filter__output(CabSession* session, const WordSetFilter* filter) {
    bool fixed_letters[MAX_PRACTICAL_WORD_LEN];
    size_t fixed_letter_index[MAX_PRACTICAL_WORD_LEN];
    for (size_t i = 0;
         i < cab_session__get_setting(*session, STG_Internal_WordLen); i++) {
        fixed_letters[i] = false;
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            if (filter->present_letters[i][j] == false) {
                continue;
            }
            if (!fixed_letters[i]) {
                fixed_letters[i] = true;
                fixed_letter_index[i] = j;
            } else {
                fixed_letters[i] = false;
                break;
            }
        }
    }

    {
        char impossible_letter = '\0';

        for (size_t i = 0; i < ALPHABET_SIZE; i++) {
            if (filter->required_letters[i] == false) {
                continue;
            }
            bool has_valid_placement = false;
            for (size_t j = 0;
                 j < cab_session__get_setting(*session, STG_Internal_WordLen);
                 j++) {
                if (fixed_letters[j] && fixed_letter_index[j] != i) {
                    continue;
                }
                if (filter->present_letters[j][i] == false) {
                    continue;
                }
                has_valid_placement = true;
            }
            if (!has_valid_placement) {
                impossible_letter = (char)('a' + (int)i);
                break;
            }
        }

        if (impossible_letter != '\0') {
            for (size_t i = 0;
                 i < cab_session__get_setting(*session, STG_Internal_WordLen);
                 i++) {
                output(session, "  [%zu] (none)\n", i + 1);
            }
            output(session,
                   "empty pattern: required letter '%c' has no valid "
                   "placement\n",
                   impossible_letter);
            return;
        }
    }

    for (size_t i = 0;
         i < cab_session__get_setting(*session, STG_Internal_WordLen); i++) {
        char not_allowed[ALPHABET_SIZE + 1];
        size_t count = 0;

        bool required_letters[ALPHABET_SIZE];
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            required_letters[j] = filter->required_letters[j];
        }
        char fixed_char;
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            if (filter->present_letters[i][j] == false) {
                not_allowed[count] = (char)('a' + (int)j);
                count++;
                required_letters[j] = false;
            } else {
                fixed_char = (char)('a' + (int)j);
            }
        }

        not_allowed[count] = '\0';

        output(session, "  [%zu] ", i + 1);
        if (count == ALPHABET_SIZE) {
            output(session, "(none)\n");
            continue;
        }
        if (count == ALPHABET_SIZE - 1) {
            output(session, "%c\n", fixed_char);
            continue;
        }

        if (count == 0) {
            output(session, "* ");
        } else {
            output(session, "!%s ", not_allowed);
        }

        char candidate_chars[ALPHABET_SIZE + 1];
        size_t candidate_count = 0;

        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            if (required_letters[j]) {
                candidate_chars[candidate_count++] = (char)('a' + (int)j);
            }
        }
        candidate_chars[candidate_count] = '\0';
        if (candidate_count > 0) {
            output(session, "?%s", candidate_chars);
        }
        output(session, "\n");
    }
}

void index_array__output(CabSession* session, IndexArray index_array,
                         const Vocabulary* vocabulary) {
    size_t j = 0;
    for (size_t i = 0; i < index_array.size; i++) {
        output(session, "%s ",
               vocabulary->words[index_array.indexes[i]].letters);
        if (++j > cab_session__get_setting(
                      *session, STG_Display_IndexArray_WordsPerLine)) {
            output(session, "\n");
            j = 0;
        }
    }
    output(session, "\n");
}

void cab_session__word_filter_init(CabSession* session) {
    if (session == NULL) {
        return;
    }
    word_set__free_content(&session->word_filter.current_word_set);
    session->word_filter.entries_count = 0;

    if (session->vocabulary != NULL) {
        word_set__init_from_vocabulary(&session->word_filter.current_word_set,
                                       session->vocabulary);
    }
}

void cab_session__word_filter_free_content(CabSession* session) {
    if (session == NULL) {
        return;
    }
    word_set__free_content(&session->word_filter.current_word_set);
    session->word_filter.entries_count = 0;
}

ListHistoryEntry cab_session__get_last_word_filter(const CabSession* session) {
    if (session == NULL || session->word_filter.entries_count == 0) {
        ListHistoryEntry default_entry = {0};
        filter__init(&default_entry.filter);
        if (session != NULL && session->vocabulary != NULL) {
            default_entry.word_count = session->vocabulary->size;
        }
        return default_entry;
    }
    return session->word_filter.history[session->word_filter.entries_count - 1];
}

size_t cab_session__compute_filter_word_count(const CabSession* session,
                                              const WordSetFilter* filter) {
    if (session == NULL || filter == NULL) {
        return 0;
    }
    IndexArray tmp = filter__get_words_from_word_set(
        &session->word_filter.current_word_set, filter);
    size_t result = tmp.size;
    index_array__free_content(&tmp);
    return result;
}

void cab_session__word_filter_add_entry(CabSession* session,
                                       ListHistoryEntry entry) {
    if (session == NULL) {
        return;
    }
    if (session->word_filter.entries_count >= HELP_FILTER_HISTORY_MAX) {
        extra_io_warning(
            session,
            "reached filter history limit! oldest filter will be deleted\n");

        for (size_t i = 0; i < HELP_FILTER_HISTORY_MAX - 1; i++) {
            session->word_filter.history[i] =
                session->word_filter.history[i + 1];
        }
        session->word_filter.history[HELP_FILTER_HISTORY_MAX - 1] = entry;
        session->word_filter.entries_count = HELP_FILTER_HISTORY_MAX;
    } else {
        session->word_filter.history[session->word_filter.entries_count] = entry;
        session->word_filter.entries_count++;
    }
}

void cab_session__word_filter_revert_to(CabSession* session,
                                        size_t history_index) {
    if (session == NULL ||
        history_index >= session->word_filter.entries_count) {
        return;
    }
    cab_session__word_filter_add_entry(
        session, session->word_filter.history[history_index]);
}

size_t cab_session__get_filter_history_size(const CabSession* session) {
    if (session == NULL) {
        return 0;
    }
    return session->word_filter.entries_count;
}

void print_current_filter(CabSession* session) {
    start_message(session, OT_FILTER);
    ListHistoryEntry cur_entry = cab_session__get_last_word_filter(session);
    output(session, "--- [%zu words] ---\n", cur_entry.word_count);
    filter__output(session, &cur_entry.filter);
    end_message(session);
}

void print_filter_history(CabSession* session) {
    const size_t history_count = cab_session__get_filter_history_size(session);
    if (history_count == 0) {
        message(session, OT_FILTER, "(no history yet)\n");
        return;
    }
    start_message(session, OT_FILTER);
    output(session, "List history (%zu entries):\n", history_count);
    for (size_t hist_idx = 0; hist_idx < history_count; hist_idx++) {
        const ListHistoryEntry entry = session->word_filter.history[hist_idx];

        output(session, "\n--- Step %zu: [%zu words] ---\n", hist_idx + 1,
               entry.word_count);
        filter__output(session, &entry.filter);
    }
    end_message(session);
}

void print_filtered_word_list(CabSession* session) {
    start_message(session, OT_LIST);
    ListHistoryEntry cur_entry = cab_session__get_last_word_filter(session);
    IndexArray filtered = filter__get_words_from_word_set(
        &session->word_filter.current_word_set, &cur_entry.filter);
    const Vocabulary* voc = session->vocabulary;
    index_array__output(session, filtered, voc);
    index_array__free_content(&filtered);
    end_message(session);
}
