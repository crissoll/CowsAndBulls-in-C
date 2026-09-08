#include <malloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#include "cab_session.h"

#include "cab_errors.h"
#include "cab_settings_api.h"
#include "cab_settings_override.h"
#include "cab_settings_values.h"
#include "cab_turns.h"

static const CabSettingId cab_setting_ids_order[STG_LEN] = {
    STG_Rule_VocabularyConstraintMode,
    STG_Rule_AttemptsCoherenceConstraintMode,
    STG_Rule_AttemptsEqualityConstraintMode,
    STG_Rule_LoseOnMaxAttemptsReached,
    STG_Display_TextWrapMaxLineLength,
    STG_Display_IndexArray_WordsPerLine,
    STG_Display_RevealSecretWordOnSurrender,
    STG_Display_RevealSecretWordOnAttemptsFinished,
    STG_Internal_WordLen,
    STG_Internal_MaxAttempts,
    STG_Internal_CommandPrefixChar,
    STG_Internal_DetectWordLenFromVocab,
    STG_Internal_AllowDuplicateLetters,
    STG_Internal_VocabDecimationPercentage,
    STG_Internal_ShowPlayAgainPrompt,
    STG_Debug_LogMode,
    STG_Debug_LogMessages,
    STG_Debug_LogInput,
    STG_Debug_LogInputPrompt,


};

typedef struct {
    size_t value;
} SettingIndex;


static SettingIndex cab_setting_id_get_index(CabSettingId setting_id) {
    for (size_t i = 0; i < STG_LEN; i++) {
        if (cab_setting_ids_order[i] == setting_id) {
            return (SettingIndex){i};
        }
    }
    return (SettingIndex){SIZE_MAX};  // I don't account for this case
}

static bool check_proper_settings_order_initialization(void) {
    bool used_settings[STG_LEN] = {0};
    for (size_t i = 0; i < STG_LEN; i++) {
        if (used_settings[cab_setting_ids_order[i]]) {
            return false;
        }
        used_settings[cab_setting_ids_order[i]] = true;
    }
    return true;
}

bool cab_settings__validate_order_initialization(void) {
    return check_proper_settings_order_initialization();
}


CabSettingEntry cab_setting_entry__new(size_t value) {
    return (CabSettingEntry){
        .value = value,
        .overridden = true,
    };
}

bool cab_setting_entry__is_overridden(CabSettingEntry entry) {
    return entry.overridden;
}

bool cab_settings_override__is_overridden(CabSettingsOverride settings,
                                          CabSettingId setting_id) {
    const SettingIndex setting_index = cab_setting_id_get_index(setting_id);
    return settings.entries[setting_index.value].overridden;
}

void cab_settings_override__set_value(CabSettingsOverride* settings,
                                      CabSettingId setting_id, size_t value) {
    const SettingIndex setting_index = cab_setting_id_get_index(setting_id);
    settings->entries[setting_index.value] = cab_setting_entry__new(value);
}


size_t cab_session__get_setting(CabSession session, CabSettingId setting) {
    const CabSettingsOverride* stg = session.settings_override;
    const SettingIndex setting_index = cab_setting_id_get_index(setting);

    if (stg == NULL ||
        cab_settings_override__is_overridden(*stg, setting) == false) {
        return cab_settings__get_default_value(setting);
    }


    return stg->entries[setting_index.value].value;
}

void cab_session__reset_setting(CabSession* session, CabSettingId setting) {
    const SettingIndex setting_index = cab_setting_id_get_index(setting);
    session->settings_override->entries[setting_index.value] =
        (CabSettingEntry){0};
}


void cab_session__reset_all_settings(CabSession* session) {
    if (session->settings_override == NULL) {
        return;
    }
    for (size_t i = 0; i < session->settings_override->settings_count; i++) {
        session->settings_override->entries[i].overridden = false;
    }
}


void cab_session__set_setting(CabSession* session, CabSettingId setting,
                              size_t value) {
    extra_io_warning(
        "cab_set_setting: trying to set setting number %d to value %zu",
        setting, value);
    switch (cab_settings__get_setting_value_validity(setting, value)) {
        case CAB_SV_NotExisting:
            extra_io_warning(
                "cab_set_setting: tried assigning non existing setting number "
                "%d\n",
                setting);
            return;

        case CAB_SV_NotInitialized:
            extra_io_warning(
                "cab_set_setting: tried assigning setting number "
                "%d; invalid setting: min_value bigger or equal to max_value\n",
                setting);
            return;

        case CAB_SV_TooLow:
            extra_io_warning(
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; this value is too low for that setting\n",
                value, setting);
            return;

        case CAB_SV_TooHigh:
            extra_io_warning(
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; this value is too high for that setting\n",
                value, setting);
            return;


        case CAB_SV_ValueNotAllowed:
            extra_io_warning(
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; value not allowed by validation function",
                value, setting);
            return;
        case CAB_SV_LockedInGame:
            if (session->current_turn != GS_FIRST_TURN &&
                session->current_turn != GS_NOT_STARTED) {
                extra_io_warning(
                    "cab_set_setting: setting number %d can only be used "
                    "before "
                    "game "
                    "starts; current game state = %d",
                    setting, session->current_turn);
                return;
            }
        case CAB_SV_Valid:
            break;
    }

    if (session->settings_override == NULL) {
        session->settings_override =
            malloc(sizeof(*session->settings_override));
        *session->settings_override =
            (CabSettingsOverride){.settings_count = STG_LEN};
    }

    cab_settings_override__set_value(session->settings_override, setting,
                                     value);
}
