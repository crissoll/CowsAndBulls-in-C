#include <stdbool.h>
#include <stdlib.h>

#include "attempts.h"

#include "cab_end.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_attempts_manager.h"

#include "cab_settings_api.h"
#include "cab_settings_override.h"


static Attempt attempts[MAX_PRACTICAL_ATTEMPTS];
size_t attempt_number = 0;
size_t invalid_attempts_number = 0;


void reset_attempts(void) {
    attempt_number = 0;
    invalid_attempts_number = 0;
}

void display_remaining_attempts(CabSession* session) {
    const size_t remaining_attempts = cab_session__get_attempts_left(session);

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

void handle_attempts_deplition(CabSession* session) {
    if (cab_session__get_setting(*session, STG_Rule_LoseOnMaxAttemptsReached) ==
            false ||
        session->ending_flags != CABEND_None) {
        return;
    }
    if (cab_session__get_attempts_left(session) == 0) {
        message(session, OT_USER,
                "reached maximum amount of attempts! you lose\n");
        if (cab_session__get_setting(
                *session, STG_Display_RevealSecretWordOnAttemptsFinished)) {
            message(session, OT_USER, "the secret word was %s\n",
                    cab_session__get_secret_word(session).letters);
        }
        return;
    }
    display_remaining_attempts(session);
}

void cab_session__add_attempt(CabSession* session, Word word,
                              GuessResult result) {
    Attempt* attempts = cab_session__get_attempts_ptr(session)->attempts;
    size_t attempts_count =
        cab_session__get_attempts_ptr(session)->valid_attempts_count;
    if (cab_session__get_attempts_left(session) == 0) {
        if (cab_get_setting(STG_Rule_LoseOnMaxAttemptsReached) == false) {
            message(session, OT_USER,
                    "reached maximum amount of attempts! oldest one will be "
                    "deleted\n");
        } else {
            // this shouldn't happen
            message(session, OT_WARNING,
                    "cab_session__add_attempt: reached branch that shouldn't "
                    "be reacheable\n");
        }
        size_t max_attempts =
            cab_session__get_setting(*session, STG_Internal_MaxAttempts);
        for (size_t i = 0; i < max_attempts - 1; i++) {
            attempts[i] = attempts[i + 1];
        }
        attempts_count = max_attempts - 1;
    }

    attempts[attempts_count] = attempt__new(word, result);
    cab_session__get_attempts_ptr(session)->valid_attempts_count =
        attempts_count + 1;

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


bool cab_attempts__contains_word(Word word, const CabAttempts* attempts) {
    for (size_t i = 0; i < attempts->valid_attempts_count; i++) {
        if (word__sort_cmp(attempts->attempts[i].word, word) == 0) {
            return true;
        }
    }
    return false;
}
