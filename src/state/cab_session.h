#ifndef CAB_STATE_SESSION
#define CAB_STATE_SESSION

#include <stddef.h>

#include "cab_end.h"
#include "cab_io_buffer.h"
#include "cab_output_buffer.h"
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
    OutputBuffer* output_buffer;             // owned temporarily
    CAB_IOBuffer* input_buffer;              // owned temporarily
    CmdTree* commands_tree;                  // owned or shared
    CabTurnId current_turn;                  // owned
    CabSettingsOverride* settings_override;  // owned or shared
    CabPaths file_paths;                     // owned
    Vocabulary* vocabulary;                  // shared
    Word* secret_word;
    CABGameEndFlags ending_flags;
} CabSession;


CabSession cab_session__new(void);
void cab_session__free_content(CabSession* session);

#endif
