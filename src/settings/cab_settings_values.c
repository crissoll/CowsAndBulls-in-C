#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#include "attempts.h"
#include "word.h"

#include "cab_constraints.h"

#include "cab_errors.h"


#include "cab_settings.h"

#include "cab_settings_values.h"

typedef bool (*SettingValueValidationFunc)(size_t value);

typedef struct {
    const char* name;
    size_t min_value;
    size_t max_value;
    size_t default_value;

} SettingsSpec;


bool validate_special_command_char(size_t value) {
    return !isalnum((unsigned char)value);
}

static const SettingsSpec setting_specs[STG_LEN] = {
    [STG_Display_IndexArray_WordsPerLine] = {"words-per-line", 0, 100, 10},
    [STG_Display_RevealSecretWordOnSurrender] =
        {"reveal-secret-word-on-surrender", false, true, true},
    [STG_Internal_WordLen] = {"word-len", 1, MAX_PRACTICAL_WORD_LEN, 5},
    [STG_Internal_DetectWordLenFromVocab] = {"set-word-len-from-vocabulary",
                                             false, true, true},
    [STG_Rule_LoseOnMaxAttemptsReached] = {"lose-on-max-attempts-reached",
                                           false, true, false},
    [STG_Internal_MaxAttempts] = {"max-attempts", 1, MAX_PRACTICAL_ATTEMPTS,
                                  MAX_PRACTICAL_ATTEMPTS},
    [STG_Display_RevealSecretWordOnAttemptsFinished] =
        {"reveal-secret-word-on-attempts-finished", false, true, true},
    [STG_Rule_VocabularyConstraintMode] = {"vocabulary-constraint-mode",
                                           CONSTR_None, CONSTR_LoseGame,
                                           CONSTR_SkipAttempt},
    [STG_Rule_AttemptsCoherenceConstraintMode] =
        {"attempts-coherence-constraint-mode", CONSTR_None, CONSTR_LoseGame,
         CONSTR_None},
    [STG_Rule_AttemptsEqualityConstraintMode] =
        {"attempts-equality-constraint-mode", CONSTR_None, CONSTR_LoseGame,
         CONSTR_SkipAttempt},
    [STG_Internal_AllowDuplicateLetters] = {"allow-duplicate-letters", false,
                                            true, true},
    [STG_Internal_VocabDecimationPercentage] =
        {"vocabulary-decimation-percentage", 0, 100, 0},
    [STG_Internal_CommandPrefixChar] = {"command-prefix-char", 0, 255, 0},
    [STG_Debug_LogMode] = {"log-mode", 0, 256, LOG_ToFile},
    [STG_Debug_LogMessages] = {"log-messages", false, true, true},
    [STG_Display_TextWrapMaxLineLength] = {"max-output-line-length", 10, 1000,
                                           80},
    [STG_Debug_LogInput] = {"log-input", false, true, true},
    [STG_Debug_LogInputPrompt] = {"log-input-prompt", false, true, true},
    [STG_Internal_ShowPlayAgainPrompt] = {"show-play-again-prompt", false, true,
                                          true},
    [STG_Debug_LogVocabularyDiscardedWords] = {"log-vocabulary-discarded-words",
                                               false, true, true},
    [STG_Debug_ShowSecretWordInSaveFiles] = {"show-secret-word-in-save-files",
                                             false, true, false},
    [STG_Debug_AddCommentsToSaveFiles] = {"add-comments-to-save-files", false,
                                          true, true},
    [STG_Debug_ReloadFileEachTurn] = {"reload-file-each-turn", false, true,
                                      true},

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

const char* cab_settings__get_name(CabSettingId setting) {
    return setting_specs[setting].name;
}
