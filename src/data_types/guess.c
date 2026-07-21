#include <stdbool.h>

#include "cab_output.h"
#include "word.h"

typedef struct {
    size_t cows;
    size_t bulls;
} GuessResult; /* result of comparing a word to a target word */

GuessResult compare_words(Word word, Word secret_word) {
    GuessResult result;
    result.cows = 0;
    result.bulls = 0;


    // bulls are counted first, and aren't be counted as cows

    bool used_secret_chars[get_word_len()];
    bool used_word_chars[get_word_len()];

    for (size_t i = 0; i < get_word_len(); i++) {
        used_secret_chars[i] = false;
        used_word_chars[i] = false;
    }

    for (size_t i = 0; i < get_word_len(); i++) {
        if (word.letters[i] == secret_word.letters[i]) {
            result.bulls++;
            used_secret_chars[i] = true;
            used_word_chars[i] = true;
            continue;
        }
    }
    for (size_t i = 0; i < get_word_len(); i++) {
        if (used_word_chars[i]) {
            continue;
        }

        for (size_t j = 0; j < get_word_len(); j++) {
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

void guess_result__output(GuessResult result) {
    output("cows: %zu, bulls: %zu", result.cows, result.bulls);
}
