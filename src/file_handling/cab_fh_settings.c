#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_session.h"
#include "cab_settings_values.h"

extern const CabSettingId cab_setting_ids_order[STG_LEN];

void cab_fh__store_settings(CabSession* session, char* buffer) {
    int offset = 0;
    if (session->settings_override != NULL) {
        size_t overridden_settings = 0;
        const size_t settings_count =
            session->settings_override->settings_count;
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden) {
                overridden_settings++;
            }
        }
        offset += sprintf(buffer + offset, "%zu\n", overridden_settings);
        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden == false) {
                continue;
            }
            const size_t val = session->settings_override->entries[i].value;
            offset += sprintf(buffer + offset, "%zu : %zu // (%s)\n", i, val,
                              cab_settings__get_name(cab_setting_ids_order[i]));
        }
    }
}

bool cab_fh__load_settings(CabSession* session, const char* buffer) {
    if (session == NULL || buffer == NULL) {
        return false;
    }

    int consumed = 0;
    int offset = 0;
    size_t overridden_settings_count;
    int params =
        sscanf(buffer + offset, "%zu%n", &overridden_settings_count, &consumed);
    if (params != 1) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load settings");
        return false;
    }
    offset += consumed;

    if (overridden_settings_count == 0) {
        if (session->settings_override != NULL) {
            free(session->settings_override);
            session->settings_override = NULL;
        }
        return true;
    }

    if (session->settings_override == NULL) {
        session->settings_override = calloc(1, sizeof(CabSettingsOverride));
        if (session->settings_override == NULL) {
            return false;
        }
    } else {
        memset(session->settings_override, 0, sizeof(CabSettingsOverride));
    }
    session->settings_override->settings_count = STG_LEN;

    for (size_t i = 0; i < overridden_settings_count; i++) {
        size_t setting_id;
        size_t val;
        consumed = 0;
        params = sscanf(buffer + offset, "%zu : %zu%n", &setting_id, &val,
                        &consumed);
        if (params != 2) {
            extra_io_warning(
                session, "cab_session__load_data: failed to load setting n.%zu",
                i);
            return false;
        }
        offset += consumed;
        if (setting_id < session->settings_override->settings_count) {
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
