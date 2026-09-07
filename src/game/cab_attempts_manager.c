#include <stdbool.h>
#include <stdlib.h>

#include "attempts.h"

#include "cab_core.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_secret_word.h"

#include "cab_attempts_manager.h"

#include "cab_settings_api.h"

extern bool is_secret_word_found(void);

static Attempt attempts[MAX_PRACTICAL_ATTEMPTS];
size_t attempt_number = 0;
size_t invalid_attempts_number = 0;

Attempt* get_attempts(void) {

    return attempts;
}

size_t get_attempt_number(void) {
    return attempt_number + invalid_attempts_number;
}

void reset_attempts(void) {
    attempt_number = 0;
    invalid_attempts_number = 0;
}

size_t get_remaining_attempts(void) {
    return get_max_attempts() - attempt_number - invalid_attempts_number;
}

void display_remaining_attempts(CabSession* session) {
    const size_t remaining_attempts = get_remaining_attempts();
    message(session, OT_USER, "you still have %d attempt%s\n",
            remaining_attempts, (remaining_attempts != 1) ? "s" : "");
}

void print_attempts(CabSession* session) {
    if (attempt_number == 0) {
        message(session, OT_ATTEMPTS, "no attempts yet!\n");
        return;
    }
    print_attempt_array(session, attempts, attempt_number);
    if (invalid_attempts_number == 1) {
        message(session, OT_ATTEMPTS, "1 invalid attempt\n");
    }
    if (invalid_attempts_number > 1) {
        message(session, OT_ATTEMPTS, "%d invalid attempts\n",
                invalid_attempts_number);
    }
    if (cab_get_setting(STG_Rule_LoseOnMaxAttemptsReached)) {
        display_remaining_attempts(session);
    }
}

bool is_word_already_attempted(Word word) {
    return is_word_in_attempt_array(word, attempts, attempt_number);
}

void compare_attempts_to_word(CabSession* session, Word word) {
    if (attempt_number == 0) {
        message(session, OT_USER, "no attempts yet!\n");
    }

    start_message(session, OT_USER);
    for (size_t i = 0; i < attempt_number; i++) {
        GuessResult expected = compare_words(attempts[i].word, word);

        attempt__output(session, attempts[i]);
        if (attempts[i].result.cows == expected.cows &&
            attempts[i].result.bulls == expected.bulls) {
            output(session, "\tV\n");
        } else {
            output(session, "\tX\t");
            output(session, "expected: ");
            guess_result__output(session, expected);
            output(session, "\n");
        }
    }
    end_message(session);
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


bool attempts_run_out(void) {
    return (attempt_number + invalid_attempts_number) >= get_max_attempts();
}

void handle_attempts_deplition(CabSession* session) {
    if (cab_get_setting(STG_Rule_LoseOnMaxAttemptsReached) == false ||
        is_secret_word_found()) {
        return;
    }
    if (attempts_run_out()) {
        message(session, OT_USER,
                "reached maximum amount of attempts! you lose\n");
        if (cab_get_setting(STG_Display_RevealSecretWordOnAttemptsFinished)) {
            message(session, OT_USER, "the secret word was %s\n",
                    get_secret_word().letters);
        }
        return;
    }
    display_remaining_attempts(session);
}

void add_attempt(CabSession* session, Word word, GuessResult result) {
    if (attempt_number >= get_max_attempts()) {
        if (cab_get_setting(STG_Rule_LoseOnMaxAttemptsReached) == false) {
            message(session, OT_USER,
                    "reached maximum amount of attempts! oldest one will be "
                    "deleted\n");
        } else {
            // this shouldn't happen
            message(session, OT_WARNING,
                    "add_attempt: reached branch that shouldn't be reacheable\n");
        }

        for (size_t i = 0; i < get_max_attempts() - 1; i++) {
            attempts[i] = attempts[i + 1];
        }
        attempt_number = get_max_attempts() - 1;
    }

    attempts[attempt_number] = attempt__new(word, result);
    attempt_number++;

    handle_attempts_deplition(session);
}

void add_invalid_attempt(CabSession* session) {
    invalid_attempts_number++;
    handle_attempts_deplition(session);
}

void init_attempts(Attempt* value, size_t _attempt_number) {
    for (size_t i = 0; i < _attempt_number; i++) {
        attempts[i] = value[i];
    }
    attempt_number = _attempt_number;
}
