#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session.h"
#include "cab_session_api.h"
#include "cab_turns.h"

const char* cab_session__play_turn(CabSession* session,
                                   const char* input_string) {
    if (cab_session__set_input(session, input_string) == INPUT_SUCCESS) {
        cab_session__process(session);
    }
    return cab_session__get_output(session);
}

#ifndef CAB_NO_GLOBAL_SESSION
const char* cab_play_turn(const char* input_string) {
    return cab_session__play_turn(cab_get_session(), input_string);
}
#endif
