#ifndef CAB_SETTINGS_API
#define CAB_SETTINGS_API

#include <stdbool.h>
#include <stddef.h>


#include "cab_settings.h"

void cab_set_setting(CabSettingId setting, size_t value);
size_t cab_get_setting(CabSettingId setting);
size_t cab_get_default_setting(CabSettingId setting);

void cab_reset_setting(CabSettingId setting);
void cab_reset_all_settings(void);


bool cab_is_setting_valid(CabSettingId setting);

#endif
