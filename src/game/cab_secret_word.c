#include "cab_secret_word.h"
#include "guess.h"

Word secret_word;


void set_secret_word(Word new_secret_word) {
    secret_word = new_secret_word;
}

GuessResult compare_with_secret_word(Word attempt) {
    return compare_words(attempt, secret_word);
}

Word get_secret_word() {
    return secret_word;
}
