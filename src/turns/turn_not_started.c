#include "cab_turns.h"

#include <stdbool.h>

#include "cab_io_consts.h"
#include "cab_output.h"

// temp solution
extern bool prompt_to_load_game(void);

const char* cab_turn_input_prompt_CAB_TID_NotStarted(CabSession* session) {
    (void)session;
    return "load previous game? (y/n)\n> ";
}

void cab_turn_process_CAB_TID_NotStarted(CabSession* session) {
    if (prompt_to_load_game()) {
        session->current_turn = CAB_TID_FirstTurn;
        return;
    }
    message(session, OT_INPUT_ERROR, "invalid input");
    session->current_turn = CAB_TID_NotStarted;
}
