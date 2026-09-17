#ifndef CAB_RAND
#define CAB_RAND
#include <stdint.h>

#include "cab_session_fwd.h"

uint32_t cab_rand(uint32_t from);

uint32_t cab_session__rand(CabSession* session);
void cab_session__rand_init(CabSession* session);

#endif
