#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "attempts.h"
#include "cab_attempts_manager.h"
#include "cab_end.h"
#include "cab_errors.h"
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_rand.h"
#include "cab_saves.h"
#include "cab_session.h"
#include "cab_settings_override.h"
#include "cab_turns.h"
#include "output_buffer_view.h"
#include "word.h"


void cab_session__init(CabSession* session) {

    session->output_buffer = malloc(sizeof(*session->output_buffer));
    if (session->output_buffer != NULL) {
        *session->output_buffer = (OutputBuffer){0};
        cab_output_buffer__init(session->output_buffer);
    }

    session->input_buffer = malloc(sizeof(*session->input_buffer));
    if (session->input_buffer != NULL) {
        *session->input_buffer = (CAB_IOBuffer){0};
        cab_io_buffer__init(session->input_buffer);
    }

    session->ending_flags = CABEND_None;

    cab_session__rand_init(session);
}

void cab_session__free_content(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->output_buffer != NULL) {
        cab_output_buffer__free_content(session->output_buffer);
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
    cab_session__free_output_buffer_view(session);
    cab_session__word_filter_free_content(session);

    free((char*)session->file_paths.saves_path);
    free((char*)session->file_paths.log_path);
    free((char*)session->file_paths.vocab_path);

    if (session->tokens.tokens != NULL) {
        free(session->tokens.tokens);
    }

    memset(session, 0, sizeof(*session));
}


bool cab_session__is_game_started(const CabSession* session) {
    switch (session->current_turn) {
        case CAB_TID_NotStarted:
        case CAB_TID_FirstTurn:
        case CAB_TID_PlayAgain:
            return false;
        case CAB_TID_Playing:
            return true;
        default:
            assert(false && "current_turn is CAB_TID_LEN or invalid");
            return false;
    }
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
        session->vocabulary->words[cab_session__rand(session) % vocab_size];
    cab_session__set_secret_word(session, secret_word);
}


CabAttempts* cab_session__get_attempts_ptr(CabSession* session) {
    return &session->attempts;
}

void cab_session__reset_attempts(CabSession* session) {
    session->attempts.valid_attempts_count = 0;
    session->attempts.invalid_attempts_count = 0;
}

size_t cab_session__get_attempts_count(CabSession* session) {
    return session->attempts.valid_attempts_count +
           session->attempts.invalid_attempts_count;
}

size_t cab_session__get_attempts_left(CabSession* session) {
    const size_t max_attempts =
        cab_session__get_setting(session, STG_Internal_MaxAttempts);
    const size_t used_attempts = cab_session__get_attempts_count(session);

    return (used_attempts >= max_attempts) ? 0 : max_attempts - used_attempts;
}


void cab_session__start_new_game(CabSession* session) {
    if (session == NULL) {
        return;
    }
    if (session->seed == 0) {
        cab_session__init(session);
    }
    cab_session__rand_init(session);
    cab_session__load_vocabulary(session);
    cab_session__generate_secret_word(session);
    cab_session__reset_attempts(session);
    cab_session__word_filter_init(session);
    cab_session__reset_end_flags(session);

    session->setup = true;
    session->loaded = false;
}


size_t cab_session__get_word_len(const CabSession* session) {
    return cab_session__get_setting(session, STG_Internal_WordLen);
}
