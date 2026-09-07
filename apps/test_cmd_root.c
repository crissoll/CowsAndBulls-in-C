#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cab_session.h"
#include "cmd_spec.h"
#include "cmd_tree.h"

// Forward declaration from cmd.h
void parse(CabSession* session, const char* tokens[], size_t token_count);

static int custom_handler_called = 0;
static int custom_no_args_called = 0;
static int custom_sub_called = 0;

static void custom_handler(CabSession* session, size_t token_count,
                           const char* tokens[]) {
    (void)session;
    (void)token_count;
    (void)tokens;
    custom_handler_called++;
}

static void custom_no_args_handler(CabSession* session) {
    (void)session;
    custom_no_args_called++;
}

static void custom_sub_handler(CabSession* session, size_t token_count,
                               const char* tokens[]) {
    (void)session;
    (void)token_count;
    (void)tokens;
    custom_sub_called++;
}

static const CommandSpec custom_sub_specs[] = {
    {
        .name = "sub",
        .case_no_args = NULL,
        .default_handler = custom_sub_handler,
        .args = NULL,
    },
    END_SPEC,
};

static const CommandSpec custom_specs[] = {
    {
        .name = "mycmd",
        .case_no_args = custom_no_args_handler,
        .default_handler = custom_handler,
        .args = custom_sub_specs,
    },
    END_SPEC,
};

static const CommandSpec custom_root = {
    .case_no_args = NULL,
    .default_handler = custom_handler,
    .args = custom_specs,
};

int main(void) {
    printf("--- Running test_cmd_root ---\n");

    // 1. Initialize session
    CabSession session = cab_session__new();
    assert(session.commands_tree == NULL);

    // 2. Default root should be returned when commands_tree is NULL
    const CommandSpec* default_root = cab_session__get_cmd_tree_root(&session);
    assert(default_root == get_default_cmd_tree_root());
    printf("[PASS] Default root retrieved successfully.\n");

    // 3. Set custom root
    cab_session_set_cmd_root(&session, &custom_root);
    assert(session.commands_tree != NULL);
    assert(cab_session__get_cmd_tree_root(&session) == &custom_root);
    printf("[PASS] Custom root set and retrieved correctly.\n");

    // 4. Test execution of custom commands through parse_command
    const char* tokens1[] = {"mycmd"};
    parse_command(&session, cab_session__get_cmd_tree_root(&session), tokens1, 1);
    assert(custom_no_args_called == 1);
    printf("[PASS] Custom no-args command executed successfully.\n");

    const char* tokens2[] = {"mycmd", "sub", "extra"};
    parse_command(&session, cab_session__get_cmd_tree_root(&session), tokens2, 3);
    assert(custom_sub_called == 1);
    printf("[PASS] Custom sub-command executed successfully.\n");

    // 5. Test disabling a command under the custom root
    assert(cab_session__is_command_allowed(session, &custom_specs[0]) == true);
    cab_session__disable_command(&session, &custom_specs[0]);
    assert(cab_session__is_command_allowed(session, &custom_specs[0]) == false);
    printf("[PASS] Command disabled successfully.\n");

    // When disabled, parse_command should not invoke the handler
    int prev_no_args_called = custom_no_args_called;
    parse_command(&session, &custom_specs[0], tokens1, 1);
    assert(custom_no_args_called == prev_no_args_called);
    printf("[PASS] Disabled command was properly blocked.\n");

    // 6. Test cleanup
    cab_session__free_content(&session);
    assert(session.commands_tree == NULL);
    printf("[PASS] Session cleanup freed commands_tree correctly.\n");

    printf("--- All tests in test_cmd_root passed! ---\n");
    return 0;
}
