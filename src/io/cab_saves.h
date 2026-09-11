#ifndef CAB_L_S
#define CAB_L_S

#include <stdbool.h>
#include <stddef.h>

#include "cab_session.h"

#define DEFAULT_VOCAB_PATH "data/words/5_letters_en_words.txt"
#define DEFAULT_SAVES_PATH "data/saves/last.saves"

bool are_save_files_valid(void);

void load_saves(void);

void store_saves(void);

void delete_save_files(void);

void cab_session__save_data(CabSession* session);

void cab_session__load_data(CabSession* session);

void cab_session__load_vocabulary(CabSession* session);
#endif
