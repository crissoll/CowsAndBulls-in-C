#ifndef CAB_HELP
#define CAB_HELP

#include "word_set.h"
#include "word_set_filter.h"

typedef struct {
	WordSetFilter filter;
	size_t word_count;
} ListHistoryEntry;

WordSet* game__help_word_set(void);
WordSetFilter* game__help_filter(void);

size_t game__help_list_history_count(void);
bool game__help_list_history_entry(size_t index, ListHistoryEntry* out_entry);
void game__help_list_history_add(size_t word_count);

void setup_help();

#endif

