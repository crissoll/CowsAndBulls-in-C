#include <stdbool.h>


#include "cab_output.h"

#include "cab_core.h"

#include "cab_attempts_manager.h"
#include "cab_secret_word.h"
#include "cab_used_vocabulary.h"


static bool secret_word_found = false;

void play_word(Word word) {
    if (!word_is_in_used_vocabulary(word)) {
        message(OT_ALERT, "word not contained in vocabulary\n");
        return;
    }
    if (is_word_already_attempted(word)) {
        message(OT_ALERT, "word already attempted\n");
        return;
    }
    GuessResult result = compare_with_secret_word(word);

    add_attempt(word, result);

    start_message(OT_GUESS_RESULT);
    guess_result__output(result);
    output("\n");
    end_message();

    if (result.bulls >= LETTERS_IN_WORD) {
        message(OT_USER,
                "Congratulations, you found the word in %zu attempts!\n",
                get_attempt_number());
        secret_word_found = true;
    }
}

bool is_secret_word_found() {
    return secret_word_found;
}
