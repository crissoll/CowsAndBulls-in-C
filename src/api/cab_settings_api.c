#include <stddef.h>

#include "cab_settings_api.h"


#include "attempts.h"
#include "cab_attempts_manager.h"
#include "cab_constraints.h"
#include "cab_io_consts.h"
#include "cab_saves.h"
#include "cmd.h"
#include "cmd_surrender.h"
#include "index_array.h"
#include "word.h"


#include "cab_output.h"

#define DEFINE_BOOL_FUNC_WRAPPER(wrapper_name, base_func) \
    static void wrapper_name(size_t value) {              \
        base_func((bool)value);                           \
    }

typedef void (*SettingsFunction)(size_t);

typedef struct {
    SettingsFunction func;
    size_t min_value;
    size_t max_value;
    size_t default_value;
} SettingsSpec;

DEFINE_BOOL_FUNC_WRAPPER(surrender_show_secret_word,
                         set_display_secret_word_on_surrender)
DEFINE_BOOL_FUNC_WRAPPER(saves_autodetect_word_len,
                         set_detect_word_len_from_voc)
DEFINE_BOOL_FUNC_WRAPPER(attempts_lose_on_limit_reached,
                         set_lose_on_attempts_finished)
DEFINE_BOOL_FUNC_WRAPPER(reveal_secret_word_on_attempts_run_out,
                         set_reveal_word_on_attempts_run_out)
DEFINE_BOOL_FUNC_WRAPPER(vocab_allow_duplicate_letters,
                         set_allow_duplicate_letters)

static SettingsSpec setting_specs[STG_LEN] = {
    [STG_Display_IndexArray_WordsPerLine] =
        {index_array__set_output_words_per_line, 0, 100, 10},
    [STG_Display_RevealSecretWordOnSurrender] = {surrender_show_secret_word, 0,
                                                 1, 1},
    [STG_Rule_LettersInWord] = {set_word_len, 1, MAX_PRACTICAL_WORD_LEN, 5},
    [STG_Internal_GetWordLenFromVocabulary] = {saves_autodetect_word_len, 0, 1,
                                               1},
    [STG_Rule_LoseOnMaxAttemptsReached] = {attempts_lose_on_limit_reached, 0, 1,
                                           0},
    [STG_Internal_MaxAttempts] = {set_max_attempts, 1, MAX_PRACTICAL_ATTEMPTS,
                                  MAX_PRACTICAL_ATTEMPTS},
    [STG_Display_RevealSecretWordOnAttemptsFinished] =
        {reveal_secret_word_on_attempts_run_out, 0, 1, 1},

    [STG_Rule_NonVocabularyGuessesConstraintMode] = {set_vocabulary_constraint,
                                                     CONSTR_None,
                                                     CONSTR_LoseGame,
                                                     CONSTR_SkipAttempt},
    [STG_Rule_PreviousAttemptsCoherencyConstraintMode] =
        {set_attempts_coherence_constraint, CONSTR_None, CONSTR_LoseGame,
         CONSTR_None},
    [STG_Rule_WordEqualToPrevAttemptConstraintMode] =
        {set_attempts_equality_constraint, CONSTR_None, CONSTR_LoseGame,
         CONSTR_SkipAttempt},
    [STG_Internal_AllowDuplicateLetters] = {vocab_allow_duplicate_letters, 0, 1,
                                            1},
    [STG_Internal_EraseRandomWordsPercentage] =
        {set_vocab_decimation_percentage, 0, 100, 0},
    [STG_Rule_SpecialCharForCommands] = {set_special_command_char_from_size_t,
                                         0, 255, 0},
};


void cab_set_setting(Settings setting, size_t value) {
    if (setting >= STG_LEN) {
        message(OT_WARNING,
                "cab_set_setting: tried assigning non existing setting with "
                "number %d\n",
                setting);
        return;
    }
    if (setting_specs[setting].func == NULL) {
        message(OT_WARNING,
                "cab_set_setting: setting number %d has no assigned function\n",
                setting);
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

    setting_specs[setting].func(value);
}
