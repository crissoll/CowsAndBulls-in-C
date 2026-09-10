#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include <stddef.h>
#include <stdint.h>

#include "cab_attempts_manager.h"
#include "cab_end.h"
#include "cab_help_filter.h"
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
#include "cab_rand.h"
#include "cab_session_cmd_tree.h"


#include "cab_settings_override.h"
#include "cab_turns.h"
#include "word.h"


typedef struct Vocabulary Vocabulary;

typedef struct {
    const char* vocab_path;
    const char* saves_path;
    const char* log_path;
} CabPaths;

typedef struct CabSession {
    OutputBuffer* output_buffer;
    CAB_IOBuffer* input_buffer;
    CabPaths file_paths;
    size_t seed;
    uint32_t rng_state;
    uint64_t file_interaction_count;

    CmdTree* commands_tree;
    Vocabulary* vocabulary;

    CabSettingsOverride* settings_override;
    Word secret_word;
    CabAttempts attempts;
    CabWordFilter word_filter;

    CabTurnId current_turn;
    CABGameEndFlags ending_flags;

} CabSession;


CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

bool cab_session__is_game_started(const CabSession* session);

Word cab_session__get_secret_word(const CabSession* session);
void cab_session__set_secret_word(CabSession* session, Word new_secret_word);

void cab_session__generate_secret_word(CabSession* session);

CabAttempts* cab_session__get_attempts_ptr(CabSession* session);

size_t cab_session__get_attempts_count(CabSession* session);

size_t cab_session__get_attempts_left(CabSession* session);
#endif
