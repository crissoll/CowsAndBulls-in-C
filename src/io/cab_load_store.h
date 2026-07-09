#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>


void load_vocabolary();

void generate_session_id();

bool are_save_files_valid();

bool load_secret_word();

void store_secret_word();

bool load_attempts();

void store_attempts();

void delete_save_files();

void store_data();

void generate_secret_word();

#endif