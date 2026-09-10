#include <stdbool.h>
#include <stddef.h>

#include "cab_attempts_manager.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_settings_override.h"
#include "cab_used_vocabulary.h"

#include "word.h"


void compare_attempts_to_first_token(CabSession* session, size_t token_count,
                                     const char* tokens[]) {
    if (token_count > 1) {
        message(session, OT_INPUT_ERROR,
                "too many arguments, only first will be used\n");
    }

    if (!can_string_be_word(session, tokens[0])) {
        return;
    }
    const size_t word_len =
        cab_session__get_setting(*session, STG_Internal_WordLen);
    Word candidate_word = word__new(tokens[0], word_len);
    if (!word_is_in_used_vocabulary(candidate_word)) {
        message(session, OT_ALERT, "word not contained in vocabulary\n");
        return;
    }
    cab_session__attempts_word_compare(session, candidate_word);
    return;
}
