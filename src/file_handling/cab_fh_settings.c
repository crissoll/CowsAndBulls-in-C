#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_session.h"
#include "cab_settings.h"
#include "cab_settings_override.h"
#include "cab_settings_values.h"

extern const CabSettingId cab_setting_ids_order[STG_LEN];

void cab_fh__store_settings(CabSession* session, char* buffer) {
    int offset = 0;
    if (session->settings_override != NULL) {
        const size_t settings_count =
            session->settings_override->settings_count;

        for (size_t i = 0; i < settings_count; i++) {
            if (session->settings_override->entries[i].overridden == false) {
                continue;
            }
            const size_t val = session->settings_override->entries[i].value;
            offset += sprintf(buffer + offset, "%zu : %zu", i, val);
            if (cab_session__get_setting(session,
                                         STG_Debug_AddCommentsToSaveFiles)) {
                offset +=
                    sprintf(buffer + offset, " // (%s)",
                            cab_settings__get_name(cab_setting_ids_order[i]));
            }
            offset += sprintf(buffer + offset, "\n");
        }
    }
}


int load_setting(CabSession* session, const char* buffer) {
    int params;
    int offset = 0;
    int consumed = 0;

    size_t setting_index;
    size_t val;

    char first_char;
    if (sscanf(buffer + offset, " %c%n", &first_char, &consumed) == 1 &&
        first_char == '-') {
        offset += consumed;
        char setting_name[100];
        params = sscanf(buffer + offset, "%99s : %zu%n", setting_name, &val,
                        &consumed);

        if (params != 2) {
            return -1;
        }
        setting_index = cab_settings__get_id_from_name(setting_name);
        if (setting_index == STG_LEN) {
            extra_io_warning(session,
                             "cab_fh__load_settings: found setting stored with "
                             "invalid name \"%s\" ",
                             setting_name);
            return -1;
        }

    } else {
        params = sscanf(buffer + offset, "%zu : %zu%n", &setting_index, &val,
                        &consumed);

        if (params != 2) {
            return -1;
        }
    }

    cab_session__set_setting(session, cab_setting_ids_order[setting_index],
                             val);
    extra_io_warning(session,
                     "cab_fh__load_settings: loaded setting %zu with value %zu",
                     setting_index, val);
    offset += consumed;
    return offset;
}
bool cab_fh__load_settings(CabSession* session, const char* buffer) {
    if (session == NULL || buffer == NULL) {
        return false;
    }

    int consumed = 0;
    int offset = 0;

    if (session->settings_override == NULL) {
        session->settings_override = calloc(1, sizeof(CabSettingsOverride));
        if (session->settings_override == NULL) {
            return false;
        }
    } else {
        memset(session->settings_override, 0, sizeof(CabSettingsOverride));
    }

    session->settings_override->settings_count = STG_LEN;

    do {
        consumed = load_setting(session, buffer + offset);
        offset += consumed;
    } while (consumed > 0);
    return true;
}

const CabFileHandler cab_fh_settings = {
    .name = "Settings",
    .load_function = cab_fh__load_settings,
    .store_function = cab_fh__store_settings,
};
