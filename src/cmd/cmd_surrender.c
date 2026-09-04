#include <stdbool.h>

#include "cab_output.h"
#include "cab_secret_word.h"
#include "cab_settings_api.h"


static bool surrendered = false;

bool has_surrendered(void) {
    return surrendered;
}

void cmd__surrender(void) {
    if (cab_get_setting(STG_Display_RevealSecretWordOnSurrender)) {
        message(OT_USER, "the secret word was %s\n", get_secret_word().letters);
    }
    surrendered = true;
}

void reset_surrender_state(void) {
    surrendered = false;
}
