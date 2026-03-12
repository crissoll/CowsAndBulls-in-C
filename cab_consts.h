#ifndef CAB_CONSTS
#define CAB_CONSTS

# define ALPHABET_SIZE 26
# define LETTERS_IN_WORD 5

# define MAX_ATTEMPTS 255

/* string literals for the word‑list filenames; using macros avoids
   multiple‑definition linker errors that occur when a header defines
   non‑`static` objects. */
# define IT_FILE_NAME "5_letters_it_words.txt"
# define EN_FILE_NAME "5_letters_en_words.txt"

# define HELP_TEXT "list [pattern] -> prints the words that follow that pattern;\n\
\twhile inside \"list\" mode you can remove words inputting new patterns, \
or quit the \"list\" mode inputting \"stop\".\n\n\
attempts -> prints the previous attempts and the results\n\n\
help -> prints this text again\n"

#endif