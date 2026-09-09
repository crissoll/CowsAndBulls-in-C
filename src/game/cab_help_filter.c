#include <stdlib.h>

#include "cab_errors.h"
#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_session_api.h"
#include "cab_used_vocabulary.h"
#include "vocabulary.h"


WordSet help_word_set;
WordSetFilter help_filter;

#define HELP_FILTER_HISTORY_MAX 100

typedef struct {
    WordSetFilter filter;
    size_t word_count;
} ListHistoryEntry;

ListHistoryEntry help_filter_history[HELP_FILTER_HISTORY_MAX];
size_t help_filter_history_size = 0;

size_t get_current_help_filter_word_count(void) {
    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,
                                                     get_current_help_filter());
    size_t result = tmp.size;
    index_array__free_content(&tmp);
    return result;
}

void add_current_filter_to_history(void) {
    const size_t word_count = get_current_help_filter_word_count();
    if (help_filter_history_size >= HELP_FILTER_HISTORY_MAX) {
        extra_io_warning(
            cab_get_session(),
            "reached filter history limit! oldest filter will be deleted\n");

        for (size_t i = 0; i < HELP_FILTER_HISTORY_MAX - 1; i++) {
            help_filter_history[i] = help_filter_history[i + 1];
        }
        help_filter_history_size = HELP_FILTER_HISTORY_MAX - 1;
    }

    help_filter_history[help_filter_history_size].filter = help_filter;
    help_filter_history[help_filter_history_size].word_count = word_count;
    help_filter_history_size++;
}

WordSetFilter* get_current_help_filter(void) {
    return &help_filter;
}

size_t get_filter_history_size(void) {
    return help_filter_history_size;
}

void revert_filter_to_history_step(size_t index) {
    help_filter = help_filter_history[index].filter;
}

static void free_word_set(WordSet* word_set) {
    for (size_t i = 0; i < MAX_PRACTICAL_WORD_LEN; i++) {
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            index_array__free_content(&word_set->words[i][j]);
        }
    }
}

void reset_list_history(void) {
    help_filter_history_size = 0;
    filter__init(&help_filter);

    free_word_set(&help_word_set);

    const Vocabulary voc = get_used_vocabulary();
    word_set__init_from_vocabulary(&help_word_set, &voc);
}

void print_current_filter(CabSession* session) {
    start_message(session, OT_FILTER);
    WordSetFilter* cur_filter = get_current_help_filter();
    const size_t word_count = get_current_help_filter_word_count();
    output(session, "--- [%zu words] ---\n", word_count);
    filter__output(session, cur_filter);
    end_message(session);
}

void print_filter_history(CabSession* session) {
    const size_t history_count = get_filter_history_size();
    if (history_count == 0) {
        message(session, OT_FILTER, "(no history yet)\n");
        return;
    }
    start_message(session, OT_FILTER);
    output(session, "List history (%zu entries):\n", history_count);
    for (size_t hist_idx = 0; hist_idx < history_count; hist_idx++) {
        const ListHistoryEntry entry = help_filter_history[hist_idx];

        output(session, "\n--- Step %zu: [%zu words] ---\n", hist_idx + 1,
               entry.word_count);
        filter__output(session, &entry.filter);
    }
    end_message(session);
}

void print_filtered_word_list(CabSession* session) {
    start_message(session, OT_LIST);
    IndexArray filtered = filter__get_words_from_word_set(
        &help_word_set, get_current_help_filter());
    const Vocabulary voc = get_used_vocabulary();
    index_array__output(session, filtered, &voc);
    index_array__free_content(&filtered);
    end_message(session);
}
