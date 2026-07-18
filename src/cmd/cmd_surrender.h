#ifndef CAB_SURRENDER
#define CAB_SURRENDER

#include <stdbool.h>


bool has_surrendered();

void cmd__surrender();

void reset_surrender_state();

void set_display_secret_word_on_surrender(bool value);

#endif
