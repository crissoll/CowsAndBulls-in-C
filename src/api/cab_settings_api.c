#include <stddef.h>

#include "cab_settings_api.h"

#include "cab_io_consts.h"
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


static SettingsSpec setting_specs[STG_LEN] = {
    [STG_Display_IndexArray_WordsPerLine] =
        {index_array__set_output_words_per_line, 0, 100, 10},
    [STG_System_RevealSecretWordOnSurrender] = {surrender_show_secret_word, 0,
                                                1, 1},
    [STG_Rule_LettersInWord] = {set_word_len, 1, 10, 5},
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
