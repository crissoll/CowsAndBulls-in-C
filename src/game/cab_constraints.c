#include "cab_constraints.h"
#include <stdbool.h>
#include "cab_attempts_manager.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_used_vocabulary.h"

static bool fatal_constraint_broke = false;

static Constraint vocabulary_constraint = CONSTR_SkipAttempt;
static Constraint attempts_coherence_constraint = CONSTR_None;
static Constraint attempts_equality_constraint = CONSTR_SkipAttempt;

bool is_fatal_constraint_broke() {
    return fatal_constraint_broke;
}

void reset_fatal_constraint_broke() {
    fatal_constraint_broke = false;
}

void set_vocabulary_constraint(size_t value) {
    vocabulary_constraint = value;
}
void set_attempts_coherence_constraint(size_t value) {
    attempts_coherence_constraint = value;
}
void set_attempts_equality_constraint(size_t value) {
    attempts_equality_constraint = value;
}


static bool handle_vocabulary_constraint(Word word) {
    if (word_is_in_used_vocabulary(word)) {
        return false;
    }
    if (vocabulary_constraint != CONSTR_None) {
        message(OT_ALERT, "word not contained in vocabulary\n");
        return true;
    }
    return false;
}

static bool handle_attempts_coherence_constraint(Word word) {
    if (word_is_compatible_with_attempts(word)) {
        return false;
    }
    if (attempts_coherence_constraint != CONSTR_None) {
        message(OT_ALERT, "word not coherent with previous attempts\n");
        return true;
    }
    return false;
}

static bool handle_attempts_equality_constraint(Word word) {
    if (is_word_already_attempted(word) == false) {
        return false;
    }
    if (attempts_equality_constraint != CONSTR_None) {
        message(OT_ALERT, "word already attempted\n");
        return true;
    }
    return false;
}

Constraint get_total_constraint(Word word) {
    if (handle_vocabulary_constraint(word)) {
        return vocabulary_constraint;
    }
    if (handle_attempts_coherence_constraint(word)) {
        return attempts_coherence_constraint;
    }

    if (handle_attempts_equality_constraint(word)) {
        return attempts_equality_constraint;
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
