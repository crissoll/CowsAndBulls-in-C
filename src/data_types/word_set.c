#include <stdio.h>
#include <stdlib.h>


#include "index_array.h"
#include "vocabulary.h"
#include "word.h"

#include "cab_errors.h"

typedef struct {
    IndexArray words[LETTERS_IN_WORD][ALPHABET_SIZE];
} WordSet;

IndexArray word_set__get_words_with_letter_at_pos(char letter,
                                                  size_t position_in_word,
                                                  WordSet words) {
    return words.words[position_in_word][(size_t)(letter - 'a')];
}

void word_set__init_from_vocabulary(WordSet* word_set,
                                    const Vocabulary* vocabulary) {
    size_t matrix[ALPHABET_SIZE][LETTERS_IN_WORD];

    vocabulary__get_words_frequencies(vocabulary, matrix);

    for (size_t i = 0; i < LETTERS_IN_WORD; i++) {
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            IndexArray arr;
            index_array__init(&arr, matrix[j][i]);
            word_set->words[i][j] = arr;
        }
    }

    for (size_t i = 0; i < vocabulary->size; i++) {
        Word word = vocabulary->words[i];
        for (size_t p = 0; p < LETTERS_IN_WORD; p++) {
            size_t letter_idx = (size_t)((unsigned char)word.letters[p] - 'a');
            if (matrix[letter_idx][p] == 0) {
                exit_with_error_message("index array too short");
            }
            IndexArray* arr = &word_set->words[p][letter_idx];
            size_t index = (arr->size - matrix[letter_idx][p]);
            arr->indexes[index] = i;
            matrix[letter_idx][p]--;
        }
    }
}

IndexArray word_set__get_words_containing_letter(const WordSet* word_set,
                                                 size_t letter_idx) {
    IndexArray result;
    index_array__init(&result, 0);

    bool have_result = false;
    for (size_t pos = 0; pos < LETTERS_IN_WORD; pos++) {
        IndexArray source = word_set->words[pos][letter_idx];
        IndexArray source_copy = index_array__copy(&source);

        if (!have_result) {
            result = source_copy;
            have_result = true;
        } else {
            IndexArray joined = join(result, source_copy);
            index_array__free_content(&result);
            index_array__free_content(&source_copy);
            result = joined;
        }
    }

    return result;
}
