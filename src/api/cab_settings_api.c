#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

#include "attempts.h"
#include "word.h"

#include "cab_constraints.h"

#include "cab_errors.h"
#include "cab_io_consts.h"
#include "cab_output.h"

#include "cab_session_api.h"

#include "cab_settings_api.h"


#define DEFINE_BOOL_FUNC_WRAPPER(wrapper_name, base_func) \
    static void wrapper_name(size_t value) {              \
        base_func((bool)value);                           \
    }

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
    [STG_Internal_VocabLoad_AutoWordLenDetection] = {false, true, true},
    [STG_Rule_LoseOnMaxAttemptsReached] = {false, true, false},
    [STG_Internal_MaxAttempts] = {1, MAX_PRACTICAL_ATTEMPTS,
                                  MAX_PRACTICAL_ATTEMPTS},
    [STG_Display_RevealSecretWordOnAttemptsFinished] = {false, true, true},
    [STG_Rule_VocabularyConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                           CONSTR_SkipAttempt},
    [STG_Rule_AttemptsCoherencyConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                                  CONSTR_None},
    [STG_Rule_AttemptsEqualityConstraintMode] = {CONSTR_None, CONSTR_LoseGame,
                                                 CONSTR_SkipAttempt},
    [STG_Internal_VocabLoad_AllowDuplicateLetters] = {false, true, true},
    [STG_Internal_VocabLoad_RandomWordsErasurePercentage] = {0, 100, 0},
    [STG_Rule_SpecialCharForCommands] = {1, 255, 0},
    [STG_Debug_LogMode] = {0, 256, LOG_ToFile},
    [STG_Debug_LogMessages] = {false, true, true},
    [STG_Display_TextWrapMaxLineLength] = {10, 1000, 80},
    [STG_Debug_LogInput] = {false, true, true},
    [STG_Debug_LogInputPrompt] = {false, true, true},
    [STG_Internal_ShowPlayAgainPrompt] = {false, true, true},

};

static const SettingValueValidationFunc validation_funcs[STG_LEN] = {
    [STG_Rule_SpecialCharForCommands] = validate_special_command_char,
};

static const bool locked_in_game_settings[STG_LEN] = {
    [STG_Internal_WordLen] = true,
    [STG_Internal_VocabLoad_AllowDuplicateLetters] = true,
    [STG_Internal_VocabLoad_RandomWordsErasurePercentage] = true,
};


bool overriden_settings[STG_LEN] = {0};
size_t settings[STG_LEN] = {0};

size_t cab_get_setting(Settings setting) {
    if (overriden_settings[setting]) {
        return settings[setting];
    }
    return setting_specs[setting].default_value;
}


void reset_setting(Settings setting) {
    overriden_settings[setting] = false;
}


void reset_all_settings(void) {
    for (size_t i = 0; i < STG_LEN; i++) {
        overriden_settings[i] = false;
    }
}


void cab_set_setting(Settings setting, size_t value) {
    extra_io_warning(
        "cab_set_setting: trying to set setting number %d to value %zu",
        setting, value);

    if (locked_in_game_settings[setting] == true &&
        cab_get_game_state() != GS_NOT_STARTED) {
        message(
            OT_WARNING,
            "cab_set_setting: setting number %d can only be used before game "
            "starts; current game state = %d",
            setting, cab_get_game_state());
        return;
    }

    if (setting >= STG_LEN) {
        extra_io_warning(
            "cab_set_setting: tried assigning non existing setting number %d\n",
            setting);
        return;
    }

    if (setting_specs[setting].min_value == setting_specs[setting].max_value) {
        message(OT_WARNING,
                "cab_set_setting: tried assigning setting number "
                "%d; invalid setting: min_value equal to max_value\n",
                setting);  // avoids access to uninitialized settings
        return;
    }

    if (value < setting_specs[setting].min_value) {
        message(OT_WARNING,
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; this value is too low for that setting\n",
                value, setting);
        return;
    }

    if (value > setting_specs[setting].max_value) {
        message(OT_WARNING,
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; this value is too high for that setting\n",
                value, setting);
        return;
    }

    if (validation_funcs[setting] != NULL &&
        validation_funcs[setting](value) == false) {
        message(OT_WARNING,
                "cab_set_setting: tried assigning value %zu to setting number "
                "%d; value not allowed by validation function",
                value,
                setting);  // TODO: create array of settings names
        return;
    }
    overriden_settings[setting] = true;
    settings[setting] = value;
}
