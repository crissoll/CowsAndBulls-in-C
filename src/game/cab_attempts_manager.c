#include "cab_attempts_manager.h"
#include "attempts.h"
#include "cab_io_consts.h"
#include "cab_output.h"


Attempt attempts[MAX_ATTEMPTS];
size_t attempt_number = 0;

size_t get_attempt_number() {
    return attempt_number;
}

void reset_attempts() {
    attempt_number = 0;
}

void print_attempts() {
    if (attempt_number == 0) {
        message(OT_ATTEMPTS, "no attempts yet!\n");
        return;
    }
    print_attempt_array(attempts, attempt_number);
}

bool is_word_already_attempted(Word word) {
    return is_word_in_attempt_array(word, attempts, attempt_number);
}

void compare_attempts_to_word(Word word) {
    size_t i;
    start_message(OT_USER);
    for (i = 0; i < attempt_number; i++) {
        GuessResult expected = compare_words(attempts[i].word, word);

        attempt__output(attempts[i]);
        if (attempts[i].result.cows == expected.cows &&
            attempts[i].result.bulls == expected.bulls) {
            output("\tV\n");
        } else {
            output("\tX\t");
            output("expected: ");
            guess_result__output(expected);
            output("\n");
        }
    }
    end_message();
}

void add_attempt(Word word, GuessResult result) {
    if (attempt_number >= MAX_ATTEMPTS) {
        message(
            OT_USER,
            "reached maximum amount of attempts! oldest one will be deleted\n");

        for (size_t i = 0; i < MAX_ATTEMPTS - 1; i++) {
            attempts[i] = attempts[i + 1];
        }
        attempt_number = MAX_ATTEMPTS - 1;
    }

    attempts[attempt_number] = attempt__new(word, result);
    attempt_number++;
}

void init_attempts(Attempt* value, size_t _attempt_number) {
    for (size_t i = 0; i < _attempt_number; i++) {
        attempts[i] = value[i];
    }
    attempt_number = _attempt_number;
}
