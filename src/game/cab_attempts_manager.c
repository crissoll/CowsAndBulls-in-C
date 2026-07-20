#include "cab_attempts_manager.h"
#include <stdbool.h>
#include <stdlib.h>
#include "attempts.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_secret_word.h"


static Attempt attempts[MAX_PRACTICAL_ATTEMPTS];
size_t attempt_number = 0;
size_t invalid_attempts_number = 0;

static bool lose_on_attempts_finished = false;

Attempt* get_attempts() {

    return attempts;
}

size_t get_attempt_number() {
    return attempt_number + invalid_attempts_number;
}

void reset_attempts() {
    attempt_number = 0;
    invalid_attempts_number = 0;
}

void print_attempts() {
    if (attempt_number == 0) {
        message(OT_ATTEMPTS, "no attempts yet!\n");
        return;
    }
    print_attempt_array(attempts, attempt_number);
    if (invalid_attempts_number == 1) {
        message(OT_ATTEMPTS, "1 invalid attempt\n");
    }
    if (invalid_attempts_number > 1) {
        message(OT_ATTEMPTS, "%d invalid attempts\n", invalid_attempts_number);
    }
    if (lose_on_attempts_finished) {
        message(OT_USER, "you still have %d attempts\n",
                get_max_attempts() - attempt_number - invalid_attempts_number);
    }
}

bool is_word_already_attempted(Word word) {
    return is_word_in_attempt_array(word, attempts, attempt_number);
}

void compare_attempts_to_word(Word word) {
    if (attempt_number == 0) {
        message(OT_USER, "no attempts yet!\n");
    }

    start_message(OT_USER);
    for (size_t i = 0; i < attempt_number; i++) {
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

bool word_is_compatible_with_attempts(Word word) {
    if (attempt_number == 0) {
        return true;
    }
    for (size_t i = 0; i < attempt_number; i++) {
        GuessResult expected = compare_words(attempts[i].word, word);

        if (attempts[i].result.cows != expected.cows ||
            attempts[i].result.bulls != expected.bulls) {
            return false;
        }
    }
    return true;
}


void set_lose_on_attempts_finished(bool value) {
    lose_on_attempts_finished = value;
}

static bool reveal_word_on_attempts_run_out = true;

void set_reveal_word_on_attempts_run_out(bool value) {
    reveal_word_on_attempts_run_out = value;
}

bool attempts_run_out() {
    return (attempt_number + invalid_attempts_number) >= get_max_attempts();
}

void handle_attempts_deplition() {
    if (lose_on_attempts_finished == false) {
        return;
    }
    if (attempts_run_out()) {
        message(OT_USER, "reached maximum amount of attempts! you lose\n");
        if (reveal_word_on_attempts_run_out) {
            message(OT_USER, "the secret word was %s\n",
                    get_secret_word().letters);
        }
        return;
    }
    message(OT_USER, "you still have %d attempts\n",
            get_max_attempts() - get_attempt_number());
}

void add_attempt(Word word, GuessResult result) {
    if (attempt_number >= get_max_attempts()) {
        message(
            OT_USER,
            "reached maximum amount of attempts! oldest one will be deleted\n");

        for (size_t i = 0; i < get_max_attempts() - 1; i++) {
            attempts[i] = attempts[i + 1];
        }
        attempt_number = get_max_attempts() - 1;
    }

    attempts[attempt_number] = attempt__new(word, result);
    attempt_number++;

    handle_attempts_deplition();
}

void add_invalid_attempt() {
    invalid_attempts_number++;
    handle_attempts_deplition();
}

void init_attempts(Attempt* value, size_t _attempt_number) {
    for (size_t i = 0; i < _attempt_number; i++) {
        attempts[i] = value[i];
    }
    attempt_number = _attempt_number;
}
