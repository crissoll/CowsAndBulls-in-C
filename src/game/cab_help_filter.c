#include <stdlib.h>

#include "cab_help_filter.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_used_vocabulary.h"


WordSet help_word_set;
WordSetFilter help_filter;

#define HELP_FILTER_HISTORY_MAX 100

typedef struct {
    WordSetFilter filter;
    size_t word_count;
} ListHistoryEntry;

ListHistoryEntry help_filter_history[HELP_FILTER_HISTORY_MAX];
size_t help_filter_history_size = 0;

size_t get_current_help_filter_word_count() {
    IndexArray tmp = filter__get_words_from_word_set(&help_word_set,
                                                     get_current_help_filter());
    size_t result = tmp.size;
    index_array__free_content(&tmp);
    return result;
}

void add_current_filter_to_history() {
    const size_t word_count = get_current_help_filter_word_count();
    if (help_filter_history_size > HELP_FILTER_HISTORY_MAX) {
        perror("help_filter_history overflowed");
        exit(EXIT_FAILURE);
    }

    help_filter_history[help_filter_history_size].filter = help_filter;
    help_filter_history[help_filter_history_size].word_count = word_count;
    help_filter_history_size++;
}

WordSetFilter* get_current_help_filter() {
    return &help_filter;
}

size_t get_filter_history_size() {
    return help_filter_history_size;
}

void revert_filter_to_history_step(size_t index) {
    help_filter = help_filter_history[index].filter;
}

void reset_list_history() {
    help_filter_history_size = 0;
    filter__init(&help_filter);

    const Vocabulary voc = get_used_vocabulary();
    word_set__init_from_vocabulary(&help_word_set, &voc);
}

void print_current_filter() {
    start_message(OT_FILTER);
    WordSetFilter* help_filter = get_current_help_filter();
    const size_t word_count = get_current_help_filter_word_count();
    output("--- [%zu words] ---\n", word_count);
    filter__output(help_filter);
    end_message();
}

void print_filter_history() {
    const size_t history_count = get_filter_history_size();
    if (history_count == 0) {
        message(OT_FILTER, "(no history yet)\n");
        return;
    }
    start_message(OT_FILTER);
    output("List history (%zu entries):\n", history_count);
    for (size_t hist_idx = 0; hist_idx < history_count; hist_idx++) {
        const ListHistoryEntry entry = help_filter_history[hist_idx];

        output("\n--- Step %zu: [%zu words] ---\n", hist_idx + 1,
               entry.word_count);
        filter__output(&entry.filter);
    }
    end_message();
}

void print_filtered_word_list() {
    start_message(OT_LIST);
    IndexArray filtered = filter__get_words_from_word_set(
        &help_word_set, get_current_help_filter());
    const Vocabulary voc = get_used_vocabulary();
    index_array__output(filtered, &voc);
    index_array__free_content(&filtered);
    end_message();
}
