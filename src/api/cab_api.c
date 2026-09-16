#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

const char* cab_play_turn(const char* input_string) {
    if (cab_session__set_input(cab_get_session(), input_string) ==
        INPUT_SUCCESS) {
        cab_process_turn();
    }
    return cab_session__get_output(cab_get_session());
}
