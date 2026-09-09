#include <stdbool.h>

#include "cab_end.h"
#include "cab_output.h"
#include "cab_secret_word.h"
#include "cab_settings_api.h"

void cmd__surrender(CabSession* session) {
    if (cab_get_setting(STG_Display_RevealSecretWordOnSurrender)) {
        message(session, OT_USER, "the secret word was %s\n",
                get_secret_word().letters);
    }
    session->ending_flags = CABEND_Surrendered;
}

void reset_surrender_state(void) {
    surrendered = false;
}
