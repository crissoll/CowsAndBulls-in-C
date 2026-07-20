#ifndef CAB_CONSTRAINTS
#define CAB_CONSTRAINTS

#include <stdbool.h>

#include "word.h"

typedef enum {
    CONSTR_None,
    CONSTR_SkipAttempt,
    CONSTR_SpendAttempt,
    CONSTR_LoseGame,
} Constraint;

typedef enum {
    Constraint_Passed,
    Constraint_Failed,
} ConstraintResult;


Constraint get_total_constraint(Word word);

ConstraintResult handle_contraints(Word word);


void set_vocabulary_constraint(size_t value);

void set_attempts_coherence_constraint(size_t value);

void set_attempts_equality_constraint(size_t value);

#endif
