
#include "cab_session_parsing.h"

void cab_session__parse_tokens_array(CabSession* session, CabTokensArray array,
                                     const CommandSpec* from_spec) {
    if (from_spec == NULL) {
        from_spec = cab_session__get_cmd_tree_root(session);
    }

    for (size_t i = 0; i < array.size; i++) {
        parse_command(session, from_spec, (const char**)array.array[i].tokens,
                      array.array[i].token_count);
    }
}
