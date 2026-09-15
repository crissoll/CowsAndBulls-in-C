#ifndef CAB_RAND
#define CAB_RAND
#include <stdint.h>

typedef struct CabSession CabSession;

uint32_t cab_rand(uint32_t from);

uint32_t cab_session__rand(CabSession* session);
void cab_session__rand_init(CabSession* session);

#endif
