#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cab_session.h"
#include "cab_session_api.h"
#include "cab_settings_api.h"
#include "cab_settings_override.h"
#include "cab_settings_values.h"
#include "cab_turns.h"

int main(void) {
    printf("--- Running test_settings ---\n");

    // 1. Validate Schema and Ordering
    assert(cab_settings__validate_order_initialization() == true);
    printf(
        "[PASS] Settings order array is fully valid with no "
        "duplicates/omissions.\n");

    // 2. Validate Default Values and Range Queries
    size_t default_word_len = cab_get_default_setting(STG_Internal_WordLen);
    assert(default_word_len == 5);
    assert(cab_is_setting_valid(STG_Internal_WordLen) == true);
    assert(cab_settings__get_min_value(STG_Internal_WordLen) == 1);
    printf("[PASS] Default settings and bounds retrieved correctly.\n");

    // 3. Test Session Isolation and Value Overrides
    CabSession session = cab_session__new();
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 5);

    // Override word length to 6
    cab_session__set_setting(&session, STG_Internal_WordLen, 6);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 6);
    assert(cab_get_default_setting(STG_Internal_WordLen) ==
           5);  // Default unchanged
    printf("[PASS] Session setting override applied successfully.\n");

    // 4. Test Reset Single Setting
    cab_session__reset_setting(&session, STG_Internal_WordLen);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 5);
    printf("[PASS] Reset single setting reverted to default.\n");

    // 5. Test Reset All Settings
    cab_session__set_setting(&session, STG_Internal_WordLen, 7);
    cab_session__set_setting(&session, STG_Display_TextWrapMaxLineLength, 120);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 7);
    assert(cab_session__get_setting(session,
                                    STG_Display_TextWrapMaxLineLength) == 120);

    cab_session__reset_all_settings(&session);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 5);
    assert(cab_session__get_setting(session,
                                    STG_Display_TextWrapMaxLineLength) == 80);
    printf("[PASS] Reset all settings reverted all overrides.\n");

    // 6. Test Validity Checker
    // A. Range validation
    assert(cab_settings__get_setting_value_validity(STG_Internal_WordLen, 0) ==
           CAB_SV_TooLow);
    assert(cab_settings__get_setting_value_validity(STG_Internal_WordLen,
                                                    500) == CAB_SV_TooHigh);
    assert(cab_settings__get_setting_value_validity(STG_Internal_WordLen, 6) ==
           CAB_SV_LockedInGame);
    assert(cab_settings__get_setting_value_validity(STG_Internal_MaxAttempts,
                                                    10) == CAB_SV_Valid);

    // B. Custom validator function: CommandPrefixChar can't be alphanumeric
    assert(cab_settings__get_setting_value_validity(
               STG_Internal_CommandPrefixChar, 'a') == CAB_SV_ValueNotAllowed);
    assert(cab_settings__get_setting_value_validity(
               STG_Internal_CommandPrefixChar, '9') == CAB_SV_ValueNotAllowed);
    assert(cab_settings__get_setting_value_validity(
               STG_Internal_CommandPrefixChar, '!') == CAB_SV_Valid);
    assert(cab_settings__get_setting_value_validity(
               STG_Internal_CommandPrefixChar, '\0') == CAB_SV_Valid);
    printf("[PASS] Range and custom value validations work accurately.\n");

    // 7. Test In-Game Lock Enforcement
    session.current_turn = CAB_TID_NotStarted;
    cab_session__set_setting(&session, STG_Internal_WordLen, 6);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) == 6);

    // When game has started, changing locked setting should be rejected
    session.current_turn = CAB_TID_Playing;
    cab_session__set_setting(&session, STG_Internal_WordLen, 4);
    assert(cab_session__get_setting(session, STG_Internal_WordLen) ==
           6);  // Retained 6
    printf("[PASS] In-game locked settings blocked during active game.\n");

    // 8. Test Global Public API wrappers
    cab_get_session();  // Ensure global session is active
    cab_set_setting(STG_Internal_MaxAttempts, 12);
    assert(cab_get_setting(STG_Internal_MaxAttempts) == 12);
    cab_reset_setting(STG_Internal_MaxAttempts);
    assert(cab_get_setting(STG_Internal_MaxAttempts) ==
           cab_get_default_setting(STG_Internal_MaxAttempts));
    cab_session_shutdown();
    printf("[PASS] Public cab_* settings API functions working.\n");

    // 9. Clean up
    cab_session__free_content(&session);
    printf("--- All tests in test_settings passed successfully! ---\n");
    return 0;
}
