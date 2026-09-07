#include <stdbool.h>


#include "cab_constraints.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_core.h"

#include "cab_attempts_manager.h"
#include "cab_secret_word.h"


static bool secret_word_found = false;


void play_word(CabSession* session, Word word) {
    const ConstraintResult constr_result = handle_contraints(session, word);
    if (constr_result == Constraint_Failed) {
        return;
    }

    GuessResult result = compare_with_secret_word(word);

    if (result.bulls >= get_word_len()) {
        message(session, OT_USER,
                "Congratulations, you found the word in %zu attempts!\n",
                get_attempt_number() + 1);
        secret_word_found = true;
    } else {
        start_message(session, OT_GUESS_RESULT);
        guess_result__output(session, result);
        end_message(session);
    }

    add_attempt(session, word, result);
}

bool is_secret_word_found(void) {
    return secret_word_found;
}


void reset_victory(void) {
    secret_word_found = false;
}
