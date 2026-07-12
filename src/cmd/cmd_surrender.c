#include <stdbool.h>

#include "cab_output.h"
#include "cab_secret_word.h"


static bool surrendered = false;

bool has_surrendered() {
    return surrendered;
}

void cmd__surrender() {

    message(OT_USER, "the secret word was %s\n", get_secret_word().letters);
    surrendered = true;
}

void reset_surrender_state() {
    surrendered = false;
}
