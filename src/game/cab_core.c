#include <stdbool.h>


#include "cab_constraints.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_core.h"

#include "cab_attempts_manager.h"
#include "cab_secret_word.h"


static bool secret_word_found = false;


void play_word(Word word) {

    const ConstraintResult constr_result = handle_contraints(word);
    if (constr_result == Constraint_Failed) {
        return;
    }

    GuessResult result = compare_with_secret_word(word);

    if (result.bulls >= get_word_len()) {
        message(OT_USER,
                "Congratulations, you found the word in %zu attempts!\n",
                get_attempt_number());
        secret_word_found = true;
        return;
    }
    start_message(OT_GUESS_RESULT);
    guess_result__output(result);
    end_message();

    add_attempt(word, result);
}

bool is_secret_word_found() {
    return secret_word_found;
}


void reset_victory() {
    secret_word_found = false;
}
