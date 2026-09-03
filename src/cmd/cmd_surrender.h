#ifndef CAB_SURRENDER
#define CAB_SURRENDER

#include <stdbool.h>


bool has_surrendered(void);

void cmd__surrender(void);

void reset_surrender_state(void);

void set_display_secret_word_on_surrender(bool value);

#endif
