#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cab_output.h"

#include "cab_core.h"

#include "cab_secret_word.h"
#include "cab_used_vocabolary.h"
#include "cab_attempts_manager.h"

static bool game_ended = false;

void play_word(Word word){
    if(is_word_already_attempted(word)){
        output("word already attempted\n");
        return;
    }
    GuessResult result = compare_with_secret_word(word);

    add_attempt(word, result);
    guess_result__print(result);
    output("\n");
    if(result.bulls >= LETTERS_IN_WORD){
        output("Congratulations, you found the word in %zu attempts!\n",get_attempt_number());
        game_ended = true;
    }
}


bool is_game_ended(){
    return game_ended;
}

