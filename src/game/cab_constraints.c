#include <stdbool.h>

#include "cab_attempts_manager.h"
#include "cab_constraints.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_settings_api.h"
#include "cab_used_vocabulary.h"


static bool fatal_constraint_broke = false;


bool is_fatal_constraint_broke(void) {
    return fatal_constraint_broke;
}

void reset_fatal_constraint_broke(void) {
    fatal_constraint_broke = false;
}


static bool handle_vocabulary_constraint(Word word) {
    if (word_is_in_used_vocabulary(word)) {
        return false;
    }
    if (cab_get_setting(STG_Rule_VocabularyConstraintMode) != CONSTR_None) {
        message(OT_ALERT, "word not contained in vocabulary\n");
        return true;
    }
    return false;
}

static bool handle_attempts_coherence_constraint(Word word) {
    if (word_is_compatible_with_attempts(word)) {
        return false;
    }
    if (cab_get_setting(STG_Rule_AttemptsCoherenceConstraintMode) !=
        CONSTR_None) {
        message(OT_ALERT, "word not coherent with previous attempts\n");
        return true;
    }
    return false;
}

static bool handle_attempts_equality_constraint(Word word) {
    if (is_word_already_attempted(word) == false) {
        return false;
    }
    if (cab_get_setting(STG_Rule_AttemptsEqualityConstraintMode) !=
        CONSTR_None) {
        message(OT_ALERT, "word already attempted\n");
        return true;
    }
    return false;
}

Constraint get_total_constraint(Word word) {
    if (handle_vocabulary_constraint(word)) {
        return cab_get_setting(STG_Rule_VocabularyConstraintMode);
    }
    if (handle_attempts_coherence_constraint(word)) {
        return cab_get_setting(STG_Rule_AttemptsCoherenceConstraintMode);
    }

    if (handle_attempts_equality_constraint(word)) {
        return cab_get_setting(STG_Rule_AttemptsEqualityConstraintMode);
    }

    return CONSTR_None;
}

ConstraintResult handle_contraints(Word word) {
    const Constraint constraint = get_total_constraint(word);
    switch (constraint) {
        case CONSTR_None:
            return Constraint_Passed;
        case CONSTR_SkipAttempt:
            return Constraint_Failed;
        case CONSTR_SpendAttempt:
            add_invalid_attempt();
            return Constraint_Failed;
        case CONSTR_LoseGame:
            fatal_constraint_broke = true;
            return Constraint_Failed;
    }
}
