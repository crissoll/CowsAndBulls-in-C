#ifndef WORD_SET_FILTER_H
#define WORD_SET_FILTER_H

#include <stdbool.h>
#include "cab_consts.h"
#include "word_set.h"
#include "index_array.h"

typedef enum {
    JOIN,
    INTERSECT,
    REMOVE,
} FilterMode;

typedef struct {
    bool present_letters[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSetFilter;

/* Initialize an empty filter (all positions allow no letters) */
void filter__init(WordSetFilter* filter);

/* Apply a pattern to the filter with the specified mode:
   - JOIN: union the pattern constraints (allow more letters)
   - INTERSECT: intersect the pattern constraints (restrict to only matching letters)
   - REMOVE: remove the pattern constraints (disallow matching letters)
   
   Supports single-letter patterns like "e" which expand to all positions. */
void filter__apply_pattern(
        WordSetFilter* filter,
        const char pattern[LETTERS_IN_WORD + 1],
        FilterMode mode
    );

/* Extract all words from the word set that match the filter constraints.
   The caller must free the returned IndexArray with index_array__free_content. */
IndexArray filter__get_words_from_word_set(
        const WordSet* word_set,
        const WordSetFilter* filter
    );

/* Print a compact, human-readable view of allowed letters by position. */
void filter__print(const WordSetFilter* filter);

#endif /* WORD_SET_FILTER_H */
