#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_session.h"


void cab_fh__store_settings(CabSession* session, char* buffer) {
    if (session->settings_override != NULL) {
        size_t overridden_settings = 0;
        const size_t settings_count =
            session->settings_override->settings_count;
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden) {
                overridden_settings++;
            }
        }
        sprintf(buffer, "%zu\n", overridden_settings);
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden == false) {
                continue;
            }
            const size_t val = session->settings_override->entries[i].value;
            sprintf(buffer, "%zu : %zu\n", i, val);
        }

    } else {
        sprintf(buffer, "0\n");
    }
}


bool cab_fh__load_settings(CabSession* session, const char* buffer) {
    int params;
    size_t overridden_settings_count;
    params = sscanf(buffer, "%zu", &overridden_settings_count);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load settings");
        return false;
    }

    if (overridden_settings_count != 0) {
        session->settings_override = calloc(1, sizeof(CabSettingsOverride));
        session->settings_override->settings_count = STG_LEN;
    }

    for (size_t i = 0; i < overridden_settings_count; i++) {
        size_t setting_id;
        size_t val;
        params = sscanf(buffer, "%zu : %zu", &setting_id, &val);
        if (params != 2) {
            extra_io_warning(
                session, "cab_session__load_data: failed to load setting n.%zu",
                i);
            return false;
        }
        if (session->settings_override != NULL &&
            setting_id < session->settings_override->settings_count) {
            session->settings_override->entries[setting_id].overridden = true;
            session->settings_override->entries[setting_id].value = val;
        }
    }
    return true;
}

const CabFileHandler cab_fh_settings = {
    .name = "Settings",
    .load_function = cab_fh__load_settings,
    .store_function = cab_fh__store_settings,
};
