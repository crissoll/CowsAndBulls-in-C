#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#include "attempts.h"
#include "word.h"

#include "cab_constraints.h"

#include "cab_errors.h"


#include "cab_settings_api.h"

#include "cab_settings_values.h"

typedef bool (*SettingValueValidationFunc)(size_t value);

typedef struct {
    size_t min_value;
    size_t max_value;
    size_t default_value;

} SettingsSpec;


bool validate_special_command_char(size_t value) {
    return !isalnum((unsigned char)value);
}

static const SettingsSpec setting_specs[STG_LEN] = {
    [STG_Display_IndexArray_WordsPerLine] = {0, 100, 10},
    [STG_Display_RevealSecretWordOnSurrender] = {false, true, true},
    [STG_Internal_WordLen] = {1, MAX_PRACTICAL_WORD_LEN, 5},
    [STG_Internal_DetectWordLenFromVocab] = {false, true, true},
    [STG_Rule_LoseOnMaxAttemptsReached] = {false, true, false},
    [STG_Internal_MaxAttempts] = {1, MAX_PRACTICAL_ATTEMPTS,
                                  MAX_PRACTICAL_ATTEMPTS},
    [STG_Display_RevealSecretWordOnAttemptsFinished] = {false, true, true},
    [STG_Rule_VocabularyConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                           CONSTR_SkipAttempt},
    [STG_Rule_AttemptsCoherenceConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                                  CONSTR_None},
    [STG_Rule_AttemptsEqualityConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                                 CONSTR_SkipAttempt},
    [STG_Internal_AllowDuplicateLetters] = {false, true, true},
    [STG_Internal_VocabDecimationPercentage] = {0, 100, 0},
    [STG_Internal_CommandPrefixChar] = {0, 255, 0},
    [STG_Debug_LogMode] = {0, 256, LOG_ToFile},
    [STG_Debug_LogMessages] = {false, true, true},
    [STG_Display_TextWrapMaxLineLength] = {10, 1000, 80},
    [STG_Debug_LogInput] = {false, true, true},
    [STG_Debug_LogInputPrompt] = {false, true, true},
    [STG_Internal_ShowPlayAgainPrompt] = {false, true, true},
    [STG_Debug_LogVocabularyDiscardedWords] = {false, true, true},

};

static const SettingValueValidationFunc validation_funcs[STG_LEN] = {
    [STG_Internal_CommandPrefixChar] = validate_special_command_char,
};

static const bool locked_in_game_settings[STG_LEN] = {
    [STG_Internal_WordLen] = true,
    [STG_Internal_AllowDuplicateLetters] = true,
    [STG_Internal_VocabDecimationPercentage] = true,
};


bool cab_setting_is_initialized(CabSettingId setting) {
    return setting_specs[setting].min_value < setting_specs[setting].max_value;
}

bool cab_setting_is_locked(CabSettingId setting) {
    return locked_in_game_settings[setting];
}

bool cab_setting_is_value_valid(CabSettingId setting, size_t value) {
    return validation_funcs[setting] == NULL ||
           validation_funcs[setting](value);
}


size_t cab_settings__get_default_value(CabSettingId setting) {
    if (setting >= STG_LEN) {
        return SIZE_MAX;
    }
    return setting_specs[setting].default_value;
}

size_t cab_settings__get_min_value(CabSettingId setting) {
    if (setting >= STG_LEN) {
        return SIZE_MAX;
    }
    return setting_specs[setting].min_value;
}

size_t cab_settings__get_max_value(CabSettingId setting) {
    if (setting >= STG_LEN) {
        return SIZE_MAX;
    }
    return setting_specs[setting].max_value;
}

CabSettingValidity cab_settings__get_setting_value_validity(
    CabSettingId setting, size_t value) {

    if (setting >= STG_LEN) {
        return CAB_SV_NotExisting;
    }

    if (!cab_setting_is_initialized(setting)) {
        return CAB_SV_NotInitialized;
    }

    if (value < setting_specs[setting].min_value) {
        return CAB_SV_TooLow;
    }

    if (value > setting_specs[setting].max_value) {
        return CAB_SV_TooHigh;
    }
    if (cab_setting_is_locked(setting)) {
        return CAB_SV_LockedInGame;
    }

    if (!cab_setting_is_value_valid(setting, value)) {
        return CAB_SV_ValueNotAllowed;
    }

    return CAB_SV_Valid;
}
