#include <stdbool.h>
#include <string.h>

#include "cab_output.h"
#include "word.h"

GuessResult compare_words(Word word, Word secret_word) {
    GuessResult result;
    result.cows = 0;
    result.bulls = 0;

    const size_t word_len = strlen(word.letters);

    bool used_secret_chars[MAX_PRACTICAL_WORD_LEN];
    bool used_word_chars[MAX_PRACTICAL_WORD_LEN];

    for (size_t i = 0; i < word_len; i++) {
        used_secret_chars[i] = false;
        used_word_chars[i] = false;
    }

    for (size_t i = 0; i < word_len; i++) {
        if (word.letters[i] == secret_word.letters[i]) {
            result.bulls++;
            used_secret_chars[i] = true;
            used_word_chars[i] = true;
            continue;
        }
    }
    for (size_t i = 0; i < word_len; i++) {
        if (used_word_chars[i]) {
            continue;
        }

        for (size_t j = 0; j < word_len; j++) {
            if (!used_secret_chars[j] &&
                word.letters[i] == secret_word.letters[j]) {
                result.cows++;
                used_secret_chars[j] = true;
                break;
            }
        }
    }
    return result;
}

void guess_result__output(CabSession* session, GuessResult result) {
    output(session, "cows: %zu, bulls: %zu", result.cows, result.bulls);
}
