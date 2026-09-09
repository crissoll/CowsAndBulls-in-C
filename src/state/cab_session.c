#include <stdbool.h>
#include <stdlib.h>

#include "attempts.h"
#include "cab_attempts_manager.h"
#include "cab_end.h"
#include "cab_errors.h"
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_rand.h"
#include "cab_session.h"
#include "cab_turns.h"
#include "cmd_spec.h"


CabSession cab_session__new(void) {
    CabSession session = {0};
    session.output_buffer = malloc(sizeof(*session.output_buffer));
    if (session.output_buffer != NULL) {
        *session.output_buffer = (OutputBuffer){0};
        cab_output_buffer__init(session.output_buffer);
    }

    session.input_buffer = malloc(sizeof(*session.input_buffer));
    if (session.input_buffer != NULL) {
        *session.input_buffer = (CAB_IOBuffer){0};
        cab_io_buffer__init(session.input_buffer);
    }

    session.ending_flags = CABEND_None;

    cab_rand_init(&session);
    return session;
}

void cab_session__free_content(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->output_buffer != NULL) {
        cab_output_buffer__free_content(*session->output_buffer);
        free(session->output_buffer);
    }
    if (session->input_buffer != NULL) {
        cab_io_buffer__free_content(session->input_buffer);
        free(session->input_buffer);
    }
    if (session->commands_tree != NULL) {
        cab_cmd_tree__free_content(session->commands_tree);
        free(session->commands_tree);
    }
    if (session->settings_override != NULL) {
        free(session->settings_override);
    }
    *session = (CabSession){0};
}


bool cab_session__is_game_started(const CabSession* session) {
    return session->current_turn != CAB_TID_NotStarted &&
           session->current_turn != CAB_TID_FirstTurn;
}

Word cab_session__get_secret_word(const CabSession* session) {
    return session->secret_word;
}

void cab_session__set_secret_word(CabSession* session, Word new_secret_word) {
    if (cab_session__is_game_started(session)) {
        extra_io_warning(session,
                         "cab_session__set_secret_word: tried assigning secret "
                         "word while the game is playing");
        return;
    }
    session->secret_word = new_secret_word;
}


void cab_session__generate_secret_word(CabSession* session) {
    size_t vocab_size = session->vocabulary->size;
    Word secret_word =
        session->vocabulary->words[cab_rand(session) % vocab_size];
    cab_session__set_secret_word(session, secret_word);
}


CabAttempts* cab_session__get_attempts_ptr(CabSession* session) {
    return &session->attempts;
}

size_t cab_session__get_attempts_count(CabSession* session) {
    return session->attempts.valid_attempts_count +
           session->attempts.invalid_attempts_count;
}

size_t cab_session__get_attempts_left(CabSession* session) {
    const size_t max_attempts =
        cab_session__get_setting(*session, STG_Internal_MaxAttempts);
    const size_t used_attempts = cab_session__get_attempts_count(session);

    return (used_attempts >= max_attempts) ? 0 : max_attempts - used_attempts;
}
