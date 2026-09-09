#include <stdbool.h>


#include "cab_attempts_manager.h"
#include "cab_constraints.h"
#include "cab_end.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_settings_api.h"
#include "cab_used_vocabulary.h"


static bool handle_vocabulary_constraint(CabSession* session, Word word) {
    if (word_is_in_used_vocabulary(word)) {
        return false;
    }
    if (cab_get_setting(STG_Rule_VocabularyConstraintMode) != CONSTR_None) {
        message(session, OT_ALERT, "word not contained in vocabulary\n");
        return true;
    }
    return false;
}

static bool handle_attempts_coherence_constraint(CabSession* session,
                                                 Word word) {
    if (cab_session__attempts_coherence(session, word)) {
        return false;
    }
    if (cab_get_setting(STG_Rule_AttemptsCoherenceConstraintMode) !=
        CONSTR_None) {
        message(session, OT_ALERT,
                "word not coherent with previous attempts\n");
        return true;
    }
    return false;
}

static bool handle_attempts_equality_constraint(CabSession* session,
                                                Word word) {
    if (cab_attempts__contains_word(
            word, cab_session__get_attempts_ptr(session)) == false) {
        return false;
    }
    if (cab_get_setting(STG_Rule_AttemptsEqualityConstraintMode) !=
        CONSTR_None) {
        message(session, OT_ALERT, "word already attempted\n");
        return true;
    }
    return false;
}

Constraint get_total_constraint(CabSession* session, Word word) {
    if (handle_vocabulary_constraint(session, word)) {
        return cab_get_setting(STG_Rule_VocabularyConstraintMode);
    }
    if (handle_attempts_coherence_constraint(session, word)) {
        return cab_get_setting(STG_Rule_AttemptsCoherenceConstraintMode);
    }

    if (handle_attempts_equality_constraint(session, word)) {
        return cab_get_setting(STG_Rule_AttemptsEqualityConstraintMode);
    }

    return CONSTR_None;
}

ConstraintResult handle_contraints(CabSession* session, Word word) {
    const Constraint constraint = get_total_constraint(session, word);
    switch (constraint) {
        case CONSTR_None:
            return Constraint_Passed;
        case CONSTR_SkipAttempt:
            return Constraint_Failed;
        case CONSTR_SpendAttempt:
            cab_session__add_invalid_attempt(session);
            return Constraint_Failed;
        case CONSTR_LoseGame:
            session->ending_flags = CABEND_FatalConstraint;
            return Constraint_Failed;
    }
}
