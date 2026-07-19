#ifndef CAB_SETTINGS
#define CAB_SETTINGS

#include <stddef.h>

typedef enum {
    STG_Rule_LettersInWord,  // if you change this you have to make sure your vocabulary matches
    STG_Rule_NonVocabularyGuessesConstraintMode,  // 0 - No constraint; 1 - Constraint; 2 - Lose; // WIP
    STG_Rule_PreviousAttemptsConstraintMode,  // 0 - No constraint; 1 - Constraint; 2 - Lose; // WIP
    STG_Rule_MaxAttemptsHandlingMode,  // 0 - Lose; 1 - Delete Oldest; // WIP

    STG_Display_IndexArray_WordsPerLine,
    //STG_Display_ColorsEnabled, //uses ANSI escape color coding // WIP
    //STG_Display_CaseFormat, // lowercase, UPPERCASE, Capitalized, Normal (capitalized only in the beginning of each sentence) // WIP

    STG_System_RevealSecretWordOnSurrender,

    STG_Internal_DetectLettersInWordFromVoc,  // sets word_len based on the first word of the voc. overrides STG_Rule_LettersInWord
    STG_Internal_AllowDuplicateLetters,  // if false, all words with duplicate letters are removed from vocabulary // WIP
    STG_Internal_MaxAttempts,     // WIP
    STG_Internal_MaxListHistory,  // WIP

    STG_LEN
} Settings;


void cab_set_setting(Settings setting, size_t value);
size_t cab_get_setting(Settings setting);
size_t cab_get_default_setting(Settings setting);


#endif
