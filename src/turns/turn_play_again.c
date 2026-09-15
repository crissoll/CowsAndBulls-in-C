#include "cab_turns.h"

#include <stdbool.h>

#include "cab_end.h"
#include "cab_input.h"
#include "cab_io_consts.h"
#include "cab_output.h"
#include "cab_rand.h"
#include "cab_session.h"
#include "cab_session_api.h"
#include "cab_settings.h"

const char* cab_turn_input_prompt_CAB_TID_PlayAgain(CabSession* session) {
    return (cab_session__get_setting(*session,
                                     STG_Internal_ShowPlayAgainPrompt))
               ? "Play Again? (y/n)\n> "
               : "Nothing more to do\n> ";
}

void cab_turn_process_CAB_TID_PlayAgain(CabSession* session) {
    if (cab_session__get_setting(*session, STG_Internal_ShowPlayAgainPrompt) ==
        false) {
        cab_session__set_end_flags(session, CABEND_DontPlayAgain);
        session->current_turn = CAB_TID_NotStarted;
        return;
    }

    switch (get_y_or_n_from_input(cab_get_session()->input_buffer)) {
        case YORN_Yes:
            cab_session__rand_init(session);
            cab_session__generate_secret_word(session);
            cab_session__reset_attempts(session);
            session->current_turn = CAB_TID_FirstTurn;
            return;
        case YORN_No:
            cab_session__set_end_flags(session, CABEND_DontPlayAgain);
            session->current_turn = CAB_TID_NotStarted;
            return;
        case YORN_Invalid:
            message(cab_get_session(), OT_INPUT_ERROR,
                    "input must be y or n\n");
            session->current_turn = CAB_TID_PlayAgain;
            return;
    }
}
