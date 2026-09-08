#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "cab_session_api.h"

#include "cab_settings_api.h"
#include "cab_settings_override.h"
#include "cab_settings_values.h"


bool cab_is_setting_valid(CabSettingId setting) {
    return cab_setting_is_initialized(setting);
}

size_t cab_get_default_setting(CabSettingId setting) {
    return cab_settings__get_default_value(setting);
}

void cab_set_setting(CabSettingId setting, size_t value) {
    cab_session__set_setting(cab_get_session(), setting, value);
}

void cab_reset_all_settings(void) {
    cab_session__reset_all_settings(cab_get_session());
}

size_t cab_get_setting(CabSettingId setting) {
    return cab_session__get_setting(*cab_get_session(), setting);
}

void cab_reset_setting(CabSettingId setting) {
    cab_session__reset_setting(cab_get_session(), setting);
}
