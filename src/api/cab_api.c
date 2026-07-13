#include <string.h>

#include "cab_api.h"
#include "cab_io_api.h"
#include "cab_paths.h"
#include "cab_session_api.h"


char* cab_play_turn(char* input_string) {
    if (cab_input(input_string) != INPUT_SUCCESS) {
        return cab_get_output();
    }
    cab_process_turn();

    return cab_get_output();
}


void cab_shutdown_game() {
    cab_session_shutdown();
    cab_io_shutdown();
}


const char* cab_get_input_prompt() {
    switch (cab_get_game_state()) {
        case GS_NOT_STARTED:
            return "load previous game? (y/n)\n> ";
        case GS_FIRST_TURN:
        case GS_PLAYING:
            return "Enter guess or command: ";
    }
}


bool cab_set_saves_folder_path(const char* path) {
    return set_saves_folder_path(path);
}


bool cab_set_vocabulary_file_path(const char* path) {
    return set_vocabulary_file_path(path);
}
