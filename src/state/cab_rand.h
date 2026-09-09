#ifndef CAB_RAND
#define CAB_RAND
#include <stdint.h>


typedef struct CabSession CabSession;

uint32_t cab_rand(CabSession* session);
void cab_rand_init(CabSession* session);

#endif
