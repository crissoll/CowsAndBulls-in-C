#include <stdint.h>
#include <time.h>

#include "cab_rand.h"
#include "cab_session.h"

uint32_t cab_rand(CabSession* session) {
    uint32_t x = session->rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    session->rng_state = x;
    return x;
}

void cab_rand_init(CabSession* session) {
    uint64_t t = (uint64_t)time(NULL);
    uint32_t seed = t % (1u << 31) ^ t >> 31;
    session->rng_state = (seed != 0) ? seed : 0x80085;
    cab_rand(session);
    session->seed = session->rng_state;
}
