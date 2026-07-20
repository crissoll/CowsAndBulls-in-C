
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_io_consts.h"

#include "cab_output.h"
#include "vocabulary.h"


static int qsort_word_cmp(const void* a, const void* b) {
    return word__sort_cmp(*(const Word*)a, *(const Word*)b);
}


void vocabulary__sort(Vocabulary* vocabulary) {
    qsort(vocabulary->words, vocabulary->size, sizeof(Word), qsort_word_cmp);
    if (vocabulary->size > 1) {
        size_t unique_count = 1;
        for (size_t i = 1; i < vocabulary->size; i++) {
            if (strcmp(vocabulary->words[i].letters,
                       vocabulary->words[unique_count - 1].letters) != 0) {
                vocabulary->words[unique_count] = vocabulary->words[i];
                unique_count++;
            } else {
                message(
                    OT_WARNING,
                    "vocabulary__sort: found duplicate word \"%s\", only one "
                    "copy will be kept\n",
                    vocabulary->words[i].letters);
            }
        }
        vocabulary->size = unique_count;
    }

    vocabulary->words =
        realloc(vocabulary->words, sizeof(Word) * vocabulary->size);
}
void vocabulary__init(Vocabulary* vocabulary, const Word* words,
                      size_t word_count) {
    if (vocabulary->words != NULL) {
        free(vocabulary->words);
        vocabulary->words = NULL;
    }

    if (words == NULL || word_count == 0) {
        vocabulary->size = 0;
        return;
    }

    vocabulary->words = malloc(word_count * sizeof(Word));
    if (vocabulary->words == NULL) {
        *vocabulary = (Vocabulary){
            .words = NULL,
            .size = 0,
        };
        message(OT_WARNING, "vocabulary__init: malloc failure\n");
        return;
    }
    memcpy(vocabulary->words, words, word_count * sizeof(Word));
    vocabulary->size = word_count;

    vocabulary__sort(vocabulary);
}


void vocabulary__get_words_frequencies(
    const Vocabulary* vocabulary,
    size_t matrix[ALPHABET_SIZE][get_word_len()]) {
    for (size_t letter_idx = 0; letter_idx < ALPHABET_SIZE; letter_idx++) {
        for (size_t pos = 0; pos < get_word_len(); pos++) {
            matrix[letter_idx][pos] = 0;
        }
    }

    for (size_t word_idx = 0; word_idx < vocabulary->size; word_idx++) {
        const Word word = vocabulary->words[word_idx];
        for (size_t pos = 0; pos < get_word_len(); pos++) {
            const unsigned char c = (unsigned char)word.letters[pos];
            if (c >= 'a' && c <= 'z') {
                matrix[(size_t)(c - 'a')][pos]++;
            }
        }
    }
}

bool vocabulary__contains_word(const Vocabulary* vocabulary, Word word) {
    if (vocabulary->size == 0) {
        return false;
    }

    size_t l = 0, r = vocabulary->size - 1;
    while (l <= r) {
        size_t m = l + (r - l) / 2;
        int cmp = word__sort_cmp(vocabulary->words[m], word);
        if (cmp == 0) {
            return true;
        }
        if (cmp < 0) {
            /* vocab[m] < word, search right half */
            l = m + 1;
        } else if (m == 0) {
            // avoids underflow of r
            return false;
        } else {
            /* vocab[m] > word, search left half */
            r = m - 1;
        }
    }
    return false;
}
