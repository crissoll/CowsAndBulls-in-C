CC := gcc
CFLAGS := -g -Wall -Wextra -std=c11 -Isrc
RM := rm -f

CAB_GAME_SRCS := \
	src/game/cab_game.c \
	src/game/cab_session.c \
	src/game/cab_guess.c \
	src/game/cab_io.c \
	src/files/cab_files.c \
	src/files/letter_dispositions.c \
	src/core/word_set.c \
	src/core/word_set_filter.c \
	src/core/index_array.c \
	src/tools/cab_data_analysis.c \
	src/core/word.c \
	src/util/utils.c \
	src/core/attempts.c

.PHONY: all clean help

all: cab_game word.o extract_5_letters_words

cab_game: $(CAB_GAME_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

word.o: src/core/word.c
	$(CC) $(CFLAGS) -c $< -o $@

extract_5_letters_words: src/tools/extract_5_letters_words.c src/game/cab_io.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) cab_game word.o extract_5_letters_words *.exe

help:
	@echo "make [target]"
	@echo "Targets: cab_game, word.o, extract_5_letters_words, clean"
