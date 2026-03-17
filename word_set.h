#ifndef WORDS   
#define WORDS

#include "cab_consts.h"
#include "index_array.h"
#include "word.h"


typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

void word_set__init_from_file(WordSet* word_set,const char* file_name);

/* The caller provides a pointer to a pre‑initialised word set and a
   pattern string of length exactly LETTERS_IN_WORD.  Each character
    should be a lowercase letter or the special `UNDEFINED_LETTER` ('*').
   Matches are words whose letters equal the pattern at every defined
   position; undefined positions are ignored.

   The implementation creates a *new* IndexArray for the result; the
   caller is responsible for freeing the returned array with
   index_array__free_content.  Internal arrays stored inside the word set
   are never freed or modified. */
IndexArray word_set__get_words_by_pattern(const WordSet *word_set, const char pattern[LETTERS_IN_WORD]);

/* Returns all words that contain the specified lowercase letter in at
    least one position. The caller must free the returned array with
    index_array__free_content. */
IndexArray word_set__get_words_containing_letter(const WordSet *word_set, char letter);

IndexArray word_set__get_words_that_contains_letters(
            const WordSet* word_set,
            const char letters[LETTERS_IN_WORD+1]
    );

#endif
