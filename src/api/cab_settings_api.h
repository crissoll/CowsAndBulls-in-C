#ifndef CAB_SETTINGS
#define CAB_SETTINGS

#include <stddef.h>

typedef enum {
    STG_Rule_VocabularyConstraintMode,  // 0 - No constraint; 1 - Constraint; 2 - Lose attempt; 3 - Lose;
    STG_Rule_AttemptsCoherencyConstraintMode,  // 0 - No constraint; 1 - Constraint; 2 - Lose attempt; 3 - Lose;
    STG_Rule_AttemptsEqualityConstraintMode,  // 0 - No constraint; 1 - Constraint; 2 - Lose attempt; 3 - Lose;
    STG_Rule_LoseOnMaxAttemptsReached,  // 0 - Delete Oldest; 1 - Lose;
    //STG_Rule_RemoveUsedWordsAcrossGames, // additional setting for possible future "campaign" mode
    STG_Rule_SpecialCharForCommands,  // any character, can't be a letter; if set to '\0', no special char will be needed

    STG_Display_TextWrapMaxLineLength,
    STG_Display_IndexArray_WordsPerLine,
    //STG_Display_ColorsEnabled, //uses ANSI escape color coding // WIP
    //STG_Display_CaseFormat, // lowercase, UPPERCASE, Capitalized, Normal (capitalized only in the beginning of each sentence) // WIP
    STG_Display_RevealSecretWordOnSurrender,
    STG_Display_RevealSecretWordOnAttemptsFinished,

    STG_Internal_WordLen,  // if you change this you have to make sure your vocabulary matches; you should also set STG_Rule_SpecialCharForCommands
    STG_Internal_VocabLoad_AutoWordLenDetection,  // sets word_len based on the first word of the voc. overrides STG_Internal_WordLen
    STG_Internal_VocabLoad_AllowDuplicateLetters,  // if false, all words with duplicate letters are removed from vocabulary
    STG_Internal_VocabLoad_RandomWordsErasurePercentage,  // remove some of the words to change the optimal words each time
    STG_Internal_MaxAttempts,
    STG_Internal_ShowPlayAgainPrompt,  // if true, player will be prompted to start a new game after finishing one

    STG_Debug_LogMode,  // bitmask: 0 - No Log; 1 - to file; 2 - to stdout;
    STG_Debug_LogMessages,
    STG_Debug_LogInput,
    STG_Debug_LogInputPrompt,


    STG_LEN
} Settings;


void cab_set_setting(Settings setting, size_t value);
size_t cab_get_setting(Settings setting);
size_t cab_get_default_setting(Settings setting);

void reset_setting(Settings setting);
void reset_all_settings(void);

#endif
