#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cab_attempts_manager.h"
#include "cab_io_consts.h"
#include "cab_output.h"


#include "attempts.h"
#include "cab_files.h"
#include "cab_session_api.h"
#include "cab_settings_api.h"
#include "cab_settings_override.h"
#include "guess.h"
#include "index_array.h"
#include "vocabulary.h"
#include "word.h"


size_t get_max_attempts(void) {
    return cab_get_setting(STG_Internal_MaxAttempts);
}


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

void store_attempt_array(const Attempt* attempts, size_t attempt_number,
                         size_t invalid_attempts_number, const char* file_name,
                         unsigned long session_id) {
    if (file_name == NULL) {
        // extra_io_warning(session, "store_attempt_array: file_name is NULL\n");
        return;
    }

    FILE* attempts_file = open_file_safe(file_name, "w");

    if (attempts_file == NULL) {
        /* extra_io_warning(session,
                         "store_attempt_array: attempts_file not found\n");
                         return;*/
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
        // extra_io_warning(session, "load_attempt_array: invalid arguments\n");
    }

    *attempt_number = 0;

    FILE* attempts_file = open_file_safe(file_name, "r");
    if (attempts_file == NULL) {
        /* extra_io_warning(session,
                         "load_attempt_array: failed to load attempts_file\n");
                         return false;*/
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
        if (cab_session__get_attempts_left(cab_get_session()) == 0) {
            break; /* prevent overflow */
        }
    }
    fclose(attempts_file);
    return true;
}
