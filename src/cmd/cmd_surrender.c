#include <stdbool.h>

#include "cab_output.h"
#include "cab_secret_word.h"


static bool surrendered = false;

bool has_surrendered() {
    return surrendered;
}

bool display_secret_word_on_surrender = true;
void cmd__surrender() {
    if (display_secret_word_on_surrender) {
        message(OT_USER, "the secret word was %s\n", get_secret_word().letters);
    }
    surrendered = true;
}

void reset_surrender_state() {
    surrendered = false;
}

void set_display_secret_word_on_surrender(bool value) {
    display_secret_word_on_surrender = value;
}
