#ifndef CAB_FH
#define CAB_FH

#include <stdbool.h>

#include "cab_session.h"

typedef struct {
    char* name;
    bool (*load_function)(CabSession*, const char*);
    void (*store_function)(CabSession*, char*);
    bool (*validation_function)(CabSession*);
} CabFileHandler;

extern const CabFileHandler cab_fh_vocabulary;
extern const CabFileHandler cab_fh_settings;
extern const CabFileHandler cab_fh_attempts;
extern const CabFileHandler cab_fh_rng;
extern const CabFileHandler cab_fh_secret_word;
extern const CabFileHandler cab_fh_cmd_disable;

static const CabFileHandler* const file_handler_list[] = {
    &cab_fh_rng,
    &cab_fh_vocabulary,
    &cab_fh_settings,
    &cab_fh_cmd_disable,
    &cab_fh_secret_word,
    &cab_fh_attempts,
    NULL,
};

#endif
