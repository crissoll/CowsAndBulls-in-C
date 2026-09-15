#include <stdint.h>
#include <time.h>

#include "cab_rand.h"
#include "cab_session.h"

uint32_t cab_rand(uint32_t from) {
    uint32_t x = from;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

uint32_t cab_session__rand(CabSession* session) {
    session->rng_state = cab_rand(session->rng_state);
    return session->rng_state;
}

void cab_session__rand_init(CabSession* session) {
    uint64_t t = (uint64_t)time(NULL);
    uint32_t seed = t % (1u << 31) ^ t >> 31;
    session->rng_state = (seed != 0) ? seed : 0x80085;

    session->seed = cab_session__rand(session);
}
