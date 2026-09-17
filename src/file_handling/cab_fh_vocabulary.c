#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_saves.h"
#include "cab_session.h"


size_t vocabulary__hash(const Vocabulary* voc) {
    if (voc == NULL) {
        return 0;
    }
    size_t hash = 0xABCD;
    for (size_t i = 0; i < voc->size; i++) {
        for (const char* p = voc->words[i].letters; *p; p++) {
            hash =
                ((hash << 5) + hash) + (unsigned char)(*p); /* hash * 33 + c */
        }
    }
    return hash;
}

void cab_fh__store_vocabulary(CabSession* session, char* buffer) {
    size_t vocab_hash = vocabulary__hash(session->vocabulary);
    sprintf(buffer, "%s %zu\n", session->file_paths.vocab_path, vocab_hash);
}


bool cab_fh__load_vocabulary(CabSession* session, const char* buffer) {
    session->file_paths.vocab_path = calloc(256, sizeof(char));
    int params =
        sscanf(buffer, "%255s %zu", (char*)session->file_paths.vocab_path,
               &session->vocab_hash);
    if (params != 2) {
        extra_io_warning(session,
                         "cab_session__load_data: failed to load vocab_path");
        cab_session__set_end_flags(session, CABEND_LoadError);
        return false;
    }

    return true;
}

bool cab_fh__validate_vocabulary(CabSession* session) {
    cab_session__load_vocabulary(session);
    size_t vocab_hash = vocabulary__hash(session->vocabulary);
    if (vocab_hash != session->vocab_hash) {
        extra_io_warning(
            session,
            "cab_session__load_data: vocabulary changed since last game");
        return false;
    }
    return true;
}

const CabFileHandler cab_fh_vocabulary = {
    .name = "Vocabulary",
    .load_function = cab_fh__load_vocabulary,
    .store_function = cab_fh__store_vocabulary,
    .validation_function = cab_fh__validate_vocabulary,
};
