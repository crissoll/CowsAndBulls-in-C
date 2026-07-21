#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "cab_io_consts.h"
#include "cab_output.h"


#include "attempts.h"
#include "cab_files.h"
#include "guess.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word.h"


static size_t max_attempts = MAX_PRACTICAL_ATTEMPTS;
size_t get_max_attempts() {
    return max_attempts;
}

void set_max_attempts(size_t value) {
    max_attempts = value;
}


Attempt attempt__new(Word word, GuessResult result) {
    Attempt attempt = {.word = word, .result = result};
    return attempt;
}

void attempt__output(Attempt attempt) {

    word__output(attempt.word);
    output("\t");
    guess_result__output(attempt.result);
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

void print_attempt_array(const Attempt* attempts, size_t attempt_number) {
    start_message(OT_ATTEMPTS);
    for (size_t i = 0; i < attempt_number; i++) {
        attempt__output(attempts[i]);
        output("\n");
    }
    end_message();
}

bool is_word_in_attempt_array(Word word, const Attempt* attempts,
                              size_t attempt_number) {
    /* return true if the given word has already been guessed earlier */
    for (size_t i = 0; i < attempt_number; i++) {
        if (word__sort_cmp(attempts[i].word, word) == 0) {
            return true;
        }
    }
    return false;
}

void store_attempt_array(const Attempt* attempts, size_t attempt_number,
                         size_t invalid_attempts_number, const char* file_name,
                         unsigned long session_id) {
    if (file_name == NULL) {
        message(OT_WARNING, "store_attempt_array: file_name is NULL");
        return;
    }

    FILE* attempts_file = open_file_safe(file_name, "w");

    if (attempts_file == NULL) {
        message(OT_WARNING, "store_attempt_array: attempts_file not found\n");
    }
    fprintf(attempts_file, "session_id %lu\n", session_id);
    fprintf(attempts_file, "invalid_attempts %zu\n", invalid_attempts_number);

    for (size_t i = 0; i < attempt_number; i++) {
        for (size_t j = 0; j < get_word_len(); j++) {
            char chr = attempts[i].word.letters[j];
            fprintf(attempts_file, "%c", chr);
        }
        fprintf(attempts_file, " %zu %zu\n", attempts[i].result.cows,
                attempts[i].result.bulls);
    }
    fclose(attempts_file);
}

bool load_attempt_array(Attempt* attempts, size_t* attempt_number,
                        size_t* invalid_attempts_number, const char* file_name,
                        unsigned long* session_id) {
    if (file_name == NULL || attempt_number == NULL || session_id == NULL) {
        message(OT_WARNING, "load_attempt_array: invalid arguments");
    }

    *attempt_number = 0;

    FILE* attempts_file = open_file_safe(file_name, "r");
    if (attempts_file == NULL) {
        message(OT_WARNING, "load_attempt_array: failed to load attempts_file");
        return false;
    }

    char label[32] = {0};

    if (fscanf(attempts_file, "%15s %lu", label, session_id) != 2 ||
        strcmp(label, "session_id") != 0) {
        fclose(attempts_file);
        return false;
    }

    if (fscanf(attempts_file, "%31s %zu", label, invalid_attempts_number) !=
            2 ||
        strcmp(label, "invalid_attempts") != 0) {
        fclose(attempts_file);
        return false;
    }

    while (true) {
        char letters[MAX_PRACTICAL_WORD_LEN + 1] = {0};
        GuessResult result;
        unsigned long cows, bulls;

        /* read a word plus cows and bulls; stop on EOF or malformed line */
        int scanned =
            fscanf(attempts_file, "%s %lu %lu", letters, &cows, &bulls);
        if (scanned != 3) {
            break;
        }

        if (!silent_can_string_be_word(letters)) {
            break;
        }

        result.cows = (size_t)cows;
        result.bulls = (size_t)bulls;

        Word word = word__new(letters);

        Attempt attempt = attempt__new(word, result);
        attempts[(*attempt_number)++] = attempt;
        if (*attempt_number >= get_max_attempts()) {
            break; /* prevent overflow */
        }
    }
    fclose(attempts_file);
    return true;
}
