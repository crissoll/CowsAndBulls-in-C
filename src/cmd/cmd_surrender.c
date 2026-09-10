#include <stdbool.h>

#include "cab_end.h"
#include "cab_output.h"
#include "cab_settings_override.h"

void cmd__surrender(CabSession* session) {
    if (cab_session__get_setting(*session,
                                 STG_Display_RevealSecretWordOnSurrender)) {
        message(session, OT_USER, "the secret word was %s\n",
                cab_session__get_secret_word(session).letters);
    }
    session->ending_flags = CABEND_Surrendered;
}
