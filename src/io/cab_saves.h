#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>

void load_vocabulary();

bool are_save_files_valid();

void load_saves();

void store_saves();

void delete_save_files();


void generate_secret_word();

void set_detect_word_len_from_voc(bool value);

void set_allow_duplicate_letters(bool value);
#endif
