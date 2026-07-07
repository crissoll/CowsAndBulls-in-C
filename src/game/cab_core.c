#include <stdbool.h>


#include "cab_output.h"

#include "cab_core.h"

#include "cab_attempts_manager.h"
#include "cab_secret_word.h"
#include "cab_used_vocabolary.h"


static bool game_ended = false;

void play_word(Word word) {
    if (!word_is_in_used_vocabolary(word)) {
        message(OT_WARNING, "word not contained in vocabolary\n");
        return;
    }
    if (is_word_already_attempted(word)) {
        message(OT_WARNING, "word already attempted\n");
        return;
    }
    GuessResult result = compare_with_secret_word(word);

    add_attempt(word, result);

    start_message(OT_GUESS_RESULT);
    guess_result__print(result);
    output("\n");
    end_message();

    if (result.bulls >= LETTERS_IN_WORD) {
        message(OT_USER,
                "Congratulations, you found the word in %zu attempts!\n",
                get_attempt_number());
        game_ended = true;
    }
}

bool is_game_ended() {
    return game_ended;
}
