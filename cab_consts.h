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



#endif