#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_attempts_manager.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#include "attempts.h"
#include "cab_settings_override.h"
#include "guess.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word.h"


Attempt attempt__new(Word word, GuessResult result) {
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}

void attempt__output(CabSession* session, Attempt attempt) {
    word__output(session, attempt.word);
    output(session, "\t");
    guess_result__output(session, attempt.result);
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

void print_attempt_array(CabSession* session, const Attempt* attempts,
                         size_t attempt_number) {
    start_message(session, OT_ATTEMPTS);
    for (size_t i = 0; i < attempt_number; i++) {
        attempt__output(session, attempts[i]);
        output(session, "\n");
    }
    end_message(session);
}
