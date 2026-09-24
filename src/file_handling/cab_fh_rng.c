#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "cab_errors.h"
#include "cab_fh.h"
#include "cab_session.h"
#include "cab_settings.h"


void cab_fh__store_rng(CabSession* session, char* buffer) {
    buffer += sprintf(buffer, "%" PRIu32, session->seed);
    if (cab_session__get_setting(session, STG_Debug_AddCommentsToSaveFiles)) {
        buffer += sprintf(buffer, " // %s", session->alpha_seed);
    }
    buffer += sprintf(buffer, "\n%" PRIu32 "\n", session->rng_state);
}

bool cab_fh__load_rng(CabSession* session, const char* buffer) {
    int params;
    uint32_t seed;
    uint32_t rng_state;
    params = sscanf(buffer, "%" SCNu32 " %" SCNu32, &seed, &rng_state);
    if (params != 2) {
        extra_io_warning(
            session,
            "cab_session__load_data: failed to load seed or rng_state");
        return false;
    }
    session->seed = seed;
    session->rng_state = rng_state;
    alpha_hash(session->seed, session->alpha_seed);
    return true;
}

const CabFileHandler cab_fh_rng = {
    .name = "Seed & RNG State",
    .load_function = cab_fh__load_rng,
    .store_function = cab_fh__store_rng,
};
