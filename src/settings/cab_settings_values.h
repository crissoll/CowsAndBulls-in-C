#ifndef CAB_SETTINGS_VALUES
#define CAB_SETTINGS_VALUES

#include <stdbool.h>
#include <stddef.h>

#include "cab_settings_api.h"

typedef enum {
    CAB_SV_Valid,
    CAB_SV_NotExisting,
    CAB_SV_NotInitialized,
    CAB_SV_TooLow,
    CAB_SV_TooHigh,
    CAB_SV_ValueNotAllowed,
    CAB_SV_LockedInGame,
} CabSettingValidity;

bool cab_setting_is_initialized(CabSettingId setting);

size_t cab_settings__get_default_value(CabSettingId setting);

size_t cab_settings__get_min_value(CabSettingId setting);

size_t cab_settings__get_max_value(CabSettingId setting);

CabSettingValidity cab_settings__get_setting_value_validity(
    CabSettingId setting, size_t value);

#endif
