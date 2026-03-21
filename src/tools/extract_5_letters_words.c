#include "game/cab_io.h"
#include <stdio.h>
#include <string.h>

int main() {
    FILE* complete_word_list = fopen("data/words/60k_it_words.txt", "r");
    if (!complete_word_list) {
        perror("Error opening input file");
        return 1;
    }
    FILE* five_letters_word_list = fopen("data/words/5_letters_it_words.txt", "w");

    if (!complete_word_list || !five_letters_word_list) {
        output("Error opening file\n");
        return 1;
    }

    char buffer[100];
    unsigned int i = 0;

    while (fgets(buffer, 100, complete_word_list) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';  // remove newline
        output("RAW: '%s' (len=%zu)\n", buffer, strlen(buffer));
        if (strlen(buffer) != 5)
            continue;

        fputs(buffer, five_letters_word_list);
        fputc('\n', five_letters_word_list);
        i++;
    }

    output("%u words\n", i);

    fclose(complete_word_list);
    fclose(five_letters_word_list);
    return 0;
}