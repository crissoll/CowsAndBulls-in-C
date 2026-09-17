#include <stdbool.h>
#include <stdio.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_session.h"

#define STR_(X) #X
#define STR(X) STR_(X)

void cab_fh__store_attempts(CabSession* session, char* buffer) {
    buffer += sprintf(buffer, "valid: %zu invalid: %zu\n",
                      session->attempts.valid_attempts_count,
                      session->attempts.invalid_attempts_count);
    for (size_t i = 0; i < session->attempts.valid_attempts_count; i++) {
        Attempt attempt = session->attempts.attempts[i];
        buffer += sprintf(buffer, "%s %zu %zu\n", attempt.word.letters,
                          attempt.result.cows, attempt.result.bulls);
    }
}

bool cab_fh__load_attempts(CabSession* session, const char* buffer) {
    int consumed = 0;
    int offset = 0;
    int params = sscanf(buffer + offset, " valid: %zu invalid: %zu%n",
                        &session->attempts.valid_attempts_count,
                        &session->attempts.invalid_attempts_count, &consumed);
    if (params != 2) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load attempts");
        return false;
    }
    offset += consumed;

    Attempt attempt;
    for (size_t i = 0; i < session->attempts.valid_attempts_count; i++) {
        params = sscanf(buffer + offset, "%" STR(MAX_PRACTICAL_WORD_LEN) "s %zu %zu%n",
                        attempt.word.letters, &attempt.result.cows,
                        &attempt.result.bulls, &consumed);
        if (params != 3) {
            extra_io_warning(
                session, "cab_session__load_data: failed to load attempt n.%zu",
                i);
            return false;
        }
        offset += consumed;
        session->attempts.attempts[i] = attempt;
    }
    return true;
}

const CabFileHandler cab_fh_attempts = {
    .name = "Attempts",
    .load_function = cab_fh__load_attempts,
    .store_function = cab_fh__store_attempts,
};
