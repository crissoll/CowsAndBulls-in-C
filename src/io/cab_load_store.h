#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>

void generate_session_id();

bool is_game_data_valid(void);

bool load_secret_word();

void store_secret_word(void);

bool load_attempts(void);

void store_attempts(void);

void delete_game_data(void);

void store_data();
#endif