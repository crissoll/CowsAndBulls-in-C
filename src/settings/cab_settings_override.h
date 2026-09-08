#ifndef CAB_SETTINGS_OVERRIDE
#define CAB_SETTINGS_OVERRIDE

#include <stdbool.h>
#include <stddef.h>

#include "cab_settings_api.h"

typedef struct CabSession CabSession;

typedef struct {
    bool overridden;
    size_t value;
} CabSettingEntry;

typedef struct {
    size_t settings_count;
    CabSettingEntry entries[STG_LEN];
} CabSettingsOverride;


size_t cab_session__get_setting(CabSession session, CabSettingId setting);

void cab_session__reset_setting(CabSession* session, CabSettingId setting);

void cab_session__reset_all_settings(CabSession* session);


void cab_session__set_setting(CabSession* session, CabSettingId setting,
                              size_t value);

bool cab_settings__validate_order_initialization(void);

#endif
