#ifndef CAB_GAME
#define CAB_GAME

#include <stdbool.h>
#include <stddef.h>

#include "core/word_set.h"
#include "core/word_set_filter.h"

typedef struct {
	WordSetFilter filter;
	size_t word_count;
} ListHistoryEntry;

void game_start();

void reset_game_vars();

bool prompt_to_load_game();

bool process_turn();

void win_game();

WordSet* game__help_word_set(void);
WordSetFilter* game__help_filter(void);

size_t game__help_list_history_count(void);
bool game__help_list_history_entry(size_t index, ListHistoryEntry* out_entry);
void game__help_list_history_add(size_t word_count);
#endif