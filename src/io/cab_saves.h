#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>
#include <stddef.h>

void load_vocabulary(void);

bool are_save_files_valid(void);

void load_saves(void);

void store_saves(void);

void delete_save_files(void);


void generate_secret_word(void);

void set_detect_word_len_from_voc(bool value);

void set_allow_duplicate_letters(bool value);

void set_vocab_decimation_percentage(size_t value);
#endif
