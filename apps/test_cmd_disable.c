#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cab_session.h"
#include "cmd_spec.h"

int main(void) {
    printf("--- Running test_cmd_disable ---\n");

    CabSession session = cab_session__new();
    const CommandSpec* root = cab_session__get_cmd_tree_root(&session);
    assert(root != NULL);

    // 1. Find command specs for 'surrender', 'attempts', 'list'
    const char* surrender_tokens[] = {"surrender"};
    const CommandSpec* surrender_spec =
        find_command_spec_in_tree(&session, 1, surrender_tokens, root);
    assert(surrender_spec != NULL);
    assert(strcmp(surrender_spec->name, "surrender") == 0);

    const char* attempts_tokens[] = {"attempts"};
    const CommandSpec* attempts_spec =
        find_command_spec_in_tree(&session, 1, attempts_tokens, root);
    assert(attempts_spec != NULL);

    const char* list_tokens[] = {"list"};
    const CommandSpec* list_spec =
        find_command_spec_in_tree(&session, 1, list_tokens, root);
    assert(list_spec != NULL);

    // 2. Initial state: all commands allowed
    assert(cab_session__is_command_allowed(session, surrender_spec) == true);
    assert(cab_session__get_command_spec_disable_flags(
               session, surrender_spec) == CMD_DISABLE_NOTHING);
    printf("[PASS] Initial flags verified.\n");

    // 3. Test: "disable surrender" -> CMD_DISABLE_ALL
    const char* disable_surrender_tokens[] = {"disable", "surrender"};
    parse_command(&session, root, disable_surrender_tokens, 2);

    assert(cab_session__is_command_allowed(session, surrender_spec) == false);
    assert(cab_session__get_command_spec_disable_flags(
               session, surrender_spec) == CMD_DISABLE_ALL);
    printf("[PASS] 'disable surrender' set CMD_DISABLE_ALL successfully.\n");

    // 4. Test: "disable --reset surrender" -> CMD_DISABLE_NOTHING
    const char* reset_surrender_tokens[] = {"disable", "--reset", "surrender"};
    parse_command(&session, root, reset_surrender_tokens, 3);

    assert(cab_session__is_command_allowed(session, surrender_spec) == true);
    assert(cab_session__get_command_spec_disable_flags(
               session, surrender_spec) == CMD_DISABLE_NOTHING);
    printf("[PASS] 'disable --reset surrender' re-enabled the command.\n");

    // 5. Test: "disable --zero-args attempts" -> CMD_DISABLE_NO_ARGS
    const char* disable_zero_args_tokens[] = {"disable", "--zero-args",
                                              "attempts"};
    parse_command(&session, root, disable_zero_args_tokens, 3);

    CabCmdDisabledFlags attempts_flags =
        cab_session__get_command_spec_disable_flags(session, attempts_spec);
    assert(attempts_flags & CMD_DISABLE_NO_ARGS);
    assert(cab_session__is_command_allowed(session, attempts_spec) == true);
    printf(
        "[PASS] 'disable --zero-args attempts' set CMD_DISABLE_NO_ARGS "
        "successfully.\n");

    // 6. Test: "disable --default attempts" -> CMD_DISABLE_DEF_HANDLER
    const char* disable_default_tokens[] = {"disable", "--default", "attempts"};
    parse_command(&session, root, disable_default_tokens, 3);

    attempts_flags =
        cab_session__get_command_spec_disable_flags(session, attempts_spec);
    assert(attempts_flags & CMD_DISABLE_DEF_HANDLER);
    assert(attempts_flags & CMD_DISABLE_NO_ARGS);
    printf(
        "[PASS] 'disable --default attempts' combined flags with "
        "CMD_DISABLE_DEF_HANDLER.\n");

    // 7. Test: "disable --args list" -> CMD_DISABLE_ARGS
    const char* disable_args_tokens[] = {"disable", "--args", "list"};
    parse_command(&session, root, disable_args_tokens, 3);

    CabCmdDisabledFlags list_flags =
        cab_session__get_command_spec_disable_flags(session, list_spec);
    assert(list_flags & CMD_DISABLE_ARGS);
    printf("[PASS] 'disable --args list' set CMD_DISABLE_ARGS successfully.\n");

    // 8. Test: Nested subcommand disabling "disable list -p"
    const char* list_p_tokens[] = {"list", "-p"};
    const CommandSpec* list_p_spec =
        find_command_spec_in_tree(&session, 2, list_p_tokens, root);
    assert(list_p_spec != NULL);
    assert(strcmp(list_p_spec->name, "-p") == 0);

    const char* disable_list_p_tokens[] = {"disable", "list", "-p"};
    parse_command(&session, root, disable_list_p_tokens, 3);

    assert(cab_session__is_command_allowed(session, list_p_spec) == false);
    printf(
        "[PASS] 'disable list -p' disabled nested subcommand successfully.\n");

    // 9. Test: Disabling nonexistent command
    const char* disable_invalid_tokens[] = {"disable", "nonexistent_cmd"};
    parse_command(&session, root, disable_invalid_tokens, 2);
    printf("[PASS] Non-existent command handled gracefully without crash.\n");

    // 10. Clean up
    cab_session__free_content(&session);
    printf("--- All tests in test_cmd_disable passed! ---\n");
    return 0;
}
