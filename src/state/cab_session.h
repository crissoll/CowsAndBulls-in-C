#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include <stddef.h>

#include "cab_attempts_manager.h"
#include "cab_end.h"
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
    OutputBuffer* output_buffer;  // owned temporarily
    CAB_IOBuffer* input_buffer;   // owned temporarily
    CabPaths file_paths;          // owned
    uint32_t rng_state;           //

    CmdTree* commands_tree;                  // owned or shared
    CabSettingsOverride* settings_override;  // owned or shared
    Vocabulary* vocabulary;                  // shared

    Word secret_word;
    CabAttempts attempts;

    CabTurnId current_turn;  // owned
    CABGameEndFlags ending_flags;

} CabSession;


CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

bool cab_session__is_game_started(const CabSession* session);

Word cab_session__get_secret_word(const CabSession* session);
void cab_session__set_secret_word(CabSession* session, Word new_secret_word);

void cab_session__generate_secret_word(CabSession* session);

#endif
