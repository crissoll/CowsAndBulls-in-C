#include <stdlib.h>

#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_session.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word_set.h"
#include "word_set_filter.h"

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
