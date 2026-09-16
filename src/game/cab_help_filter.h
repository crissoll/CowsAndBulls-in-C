#ifndef CAB_HELP
#define CAB_HELP

#include <stddef.h>

#include "word_set.h"
#include "word_set_filter.h"

typedef struct CabSession CabSession;

#define HELP_FILTER_HISTORY_MAX 100

typedef struct {
    WordSetFilter filter;
    size_t word_count;
} ListHistoryEntry;

typedef struct {
    ListHistoryEntry history[HELP_FILTER_HISTORY_MAX];
    size_t entries_count;
    WordSet current_word_set;
    ListHistoryEntry default_entry;
} CabWordFilter;

void cab_session__word_filter_init(CabSession* session);

void cab_session__word_filter_free_content(CabSession* session);

const ListHistoryEntry* cab_session__get_last_word_filter(CabSession* session);

size_t cab_session__compute_filter_word_count(const CabSession* session,
                                              const WordSetFilter* filter);

void cab_session__word_filter_add_entry(CabSession* session,
                                        const ListHistoryEntry* entry);

void cab_session__word_filter_revert_to(CabSession* session,
                                        size_t history_index);

size_t cab_session__get_filter_history_size(const CabSession* session);

void filter__output(CabSession* session, const WordSetFilter* filter);

void index_array__output(CabSession* session, IndexArray index_array,
                         const Vocabulary* vocabulary);

void print_current_filter(CabSession* session);

void print_filter_history(CabSession* session);

void print_filtered_word_list(CabSession* session);

#endif
