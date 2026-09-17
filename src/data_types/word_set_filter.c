#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "index_array.h"
#include "word.h"
#include "word_set.h"
#include "word_set_filter.h"

void filter__init(WordSetFilter* filter) {
    for (size_t i = 0; i < MAX_PRACTICAL_WORD_LEN; i++) {
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            filter->present_letters[i][j] = true;
        }
    }

    for (size_t i = 0; i < ALPHABET_SIZE; i++) {
        filter->required_letters[i] = false;
    }
}

void filter__apply_pattern(WordSetFilter* filter, const char* pattern,
                           FilterMode mode) {
    if (strlen(pattern) == 1) {
        const size_t letter_idx = (size_t)(pattern[0] - 'a');
        if (mode == REMOVE) {
            for (size_t i = 0; i < MAX_PRACTICAL_WORD_LEN; i++) {
                filter->present_letters[i][letter_idx] = false;
            }
        } else {
            filter->required_letters[letter_idx] = true;
        }
        return;
    }

    for (size_t i = 0; i < MAX_PRACTICAL_WORD_LEN && pattern[i] != '\0'; i++) {
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            switch (mode) {
                case JOIN:
                    if (pattern[i] == (char)('a' + (char)j)) {
                        filter->present_letters[i][j] = true;
                    }
                    break;
                case INTERSECT:
                    /* Only apply constraint if pattern specifies a letter (not
           wildcard). Wildcard means "don't constrain this position". */
                    if (pattern[i] != UNDEFINED_LETTER) {
                        if (filter->present_letters[i][j]) {
                            filter->present_letters[i][j] =
                                (pattern[i] == (char)('a' + (char)j));
                        }
                    }
                    break;
                case REMOVE:
                    /* If pattern specifies a letter, remove that letter from
           this position. If pattern has wildcard, don't remove anything from
           this position. */
                    if (pattern[i] != UNDEFINED_LETTER &&
                        filter->present_letters[i][j]) {
                        filter->present_letters[i][j] =
                            (pattern[i] != (char)('a' + (char)j));
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

IndexArray filter__get_words_from_word_set(const WordSet* word_set,
                                           const WordSetFilter* filter,
                                           size_t word_len) {
    IndexArray result;
    index_array__init(&result, 0);

    bool have_result = false;

    /* For each position, union words with allowed letters, then intersect
     * across positions */
    for (size_t i = 0; i < word_len; i++) {
        IndexArray position_result;
        index_array__init(&position_result, 0);

        bool have_position_result = false;

        /* Union all word indices where position i has an allowed letter */
        for (size_t j = 0; j < ALPHABET_SIZE; j++) {
            if (filter->present_letters[i][j]) {
                IndexArray source = word_set->words[i][j];
                IndexArray temp = index_array__copy(&source);

                if (!have_position_result) {
                    position_result = temp;
                    have_position_result = true;
                } else {
                    IndexArray new_position = join(position_result, temp);
                    index_array__free_content(&position_result);
                    index_array__free_content(&temp);
                    position_result = new_position;
                }
            }
        }

        /* Intersect this position's result with overall result */
        if (have_position_result) {
            if (!have_result) {
                result = position_result;
                have_result = true;
            } else {
                IndexArray new_result = intersect(result, position_result);
                index_array__free_content(&result);
                index_array__free_content(&position_result);
                result = new_result;
            }
        }
    }

    for (size_t letter_idx = 0; letter_idx < ALPHABET_SIZE; letter_idx++) {
        if (filter->required_letters[letter_idx]) {
            IndexArray has_letter =
                word_set__get_words_containing_letter(word_set, letter_idx);
            IndexArray updated = intersect(result, has_letter);
            index_array__free_content(&result);
            index_array__free_content(&has_letter);
            result = updated;
        }
    }

    return result;
}
