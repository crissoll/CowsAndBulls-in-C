#include <stddef.h>

#include "attempts.h"
#include "guess.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word.h"

Attempt attempt__new(Word word, GuessResult result) {
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}

IndexArray get_possible_words_from_attempt(Attempt attempt,
                                           const Vocabulary* vocabulary) {
    IndexArray result;

    /* allocate the maximum possible size; we'll trim by updating result.size */
    index_array__init(&result, vocabulary->size);

    size_t count = 0;
    for (size_t i = 0; i < vocabulary->size; i++) {
        Word candidate = vocabulary->words[i];
        GuessResult r = compare_words(attempt.word, candidate);
        if (r.bulls == attempt.result.bulls && r.cows == attempt.result.cows) {
            result.indexes[count++] = i;
        }
    }
    result.size = count;

    return result;
}
