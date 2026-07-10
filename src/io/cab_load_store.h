#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>

void load_vocabolary();

bool are_save_files_valid();

// tries loading save data; if it doesn't find them, it returns false
void load_saves();

void store_saves();

void delete_save_files();


void generate_secret_word();

#endif