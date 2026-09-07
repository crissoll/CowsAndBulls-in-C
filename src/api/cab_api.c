#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_session_api.h"

char* cab_play_turn(char* input_string) {
    if (cab_input(cab_get_session(), input_string) != INPUT_SUCCESS) {
        return cab_get_output(cab_get_session());
    }
    cab_process_turn();

    return cab_get_output(cab_get_session());
}
