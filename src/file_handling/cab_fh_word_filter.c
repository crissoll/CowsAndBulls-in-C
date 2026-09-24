#include <stdbool.h>

#include "cab_fh.h"
#include "cab_output.h"
#include "cab_session.h"
#include "cab_session_parsing.h"
#include "cab_tokens.h"


void cab_fh__store_word_list(CabSession* session, char* buffer) {
    const CabTokensArray* tokens_arr = &session->word_filter.word_filter_tokens;
    cab_tokens_array__store(tokens_arr, buffer);
}


bool cab_fh__load_word_list(CabSession* session, const char* buffer) {
    if (buffer == NULL) {
        return true;
    }
    CabTokensArray* token_array = &session->word_filter.word_filter_tokens;
    cab_tokens_array__load(token_array, buffer);
    return true;
}

bool cab_fh__validate_word_list(CabSession* session) {
    const CommandSpec* root = cab_session__get_cmd_tree_root(session);

    CabTokensArray array = session->word_filter.word_filter_tokens;
    session->word_filter.word_filter_tokens = (CabTokensArray){0};

    silence_messages(session);
    const char* text = "list";
    const CommandSpec* spec =
        find_command_spec_in_tree(session, 1, &text, root);
    cab_session__parse_tokens_array(session, array, spec);
    unsilence_messages(session);
    session->word_filter.word_filter_tokens = array;
    return true;
}

const CabFileHandler cab_fh_word_filter = {
    .name = "Word List",
    .load_function = cab_fh__load_word_list,
    .store_function = cab_fh__store_word_list,
    .validation_function = cab_fh__validate_word_list,
};
