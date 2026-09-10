#include <stdbool.h>


#include "cab_constraints.h"
#include "cab_end.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_core.h"

#include "cab_attempts_manager.h"
#include "cab_settings_override.h"

void play_word(CabSession* session, Word word) {
    const ConstraintResult constr_result = handle_contraints(session, word);
    if (constr_result == Constraint_Failed) {
        return;
    }

    GuessResult result =
        compare_words(word, cab_session__get_secret_word(session));
    size_t word_len = cab_session__get_setting(*session, STG_Internal_WordLen);
    if (result.bulls >= word_len) {
        message(session, OT_USER,
                "Congratulations, you found the word in %zu attempts!\n",
                cab_session__get_attempts_ptr(session)->valid_attempts_count);
        session->ending_flags = CABEND_SecretWordFound;
    } else {
        start_message(session, OT_GUESS_RESULT);
        guess_result__output(session, result);
        end_message(session);
    }

    cab_session__add_attempt(session, word, result);
}
