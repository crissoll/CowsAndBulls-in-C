CC := clang
RM := rm -f
AR := ar rcs
COMPILE_COMMANDS := compile_commands.json
comma := ,

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

CAB_GAME_APP_SRCS := \
	apps/api_usage_example.c

CAB_GAME_SHARED_SRCS := $(call rwildcard,src/,*.c)

CAB_GAME_EXEC_SRCS := $(CAB_GAME_APP_SRCS) $(CAB_GAME_SHARED_SRCS)
CAB_GAME_LIB_OBJS := $(CAB_GAME_SHARED_SRCS:.c=.o)

# Compute include directories from all source file directories so the Makefile
# keeps working when adding/removing subfolders under `src/` or `apps/`.
SRCDIRS := $(sort $(dir $(CAB_GAME_EXEC_SRCS)))
INCLUDES := $(addprefix -I,$(SRCDIRS))
CFLAGS := -g -Wall -Wextra -std=c11 $(INCLUDES)
DEBUG_CFLAGS := -O0 -g -Wall -Wextra -std=c11 $(INCLUDES)

json_escape = $(subst \,\\,$(subst ",\",$(1)))

define compile_command_entry
{ "directory": "$(call json_escape,$(CURDIR))", "command": "$(call json_escape,$(CC) $(CFLAGS) -c $(1) -o $(basename $(1)).o)", "file": "$(call json_escape,$(abspath $(1)))" }
endef

.PHONY: all clean help game game-lib debug compile_commands.json api_example_lib

all: cab_game word.o

game: game-lib cab_game

game-lib: libcab_game.a

debug: CFLAGS := $(DEBUG_CFLAGS)
debug: cab_game
	./cab_game

$(COMPILE_COMMANDS):
	@$(file >$(COMPILE_COMMANDS),[)
	@$(foreach src,$(CAB_GAME_EXEC_SRCS),$(file >>$(COMPILE_COMMANDS),$(call compile_command_entry,$(src)))$(if $(filter $(src),$(lastword $(CAB_GAME_EXEC_SRCS))),,$(file >>$(COMPILE_COMMANDS),$(comma))))
	@$(file >>$(COMPILE_COMMANDS),])
	@:

cab_game: $(CAB_GAME_EXEC_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

libcab_game.a: $(CAB_GAME_LIB_OBJS)
	$(AR) $@ $^

api_example_lib: libcab_game.a
	$(CC) $(CFLAGS) -o apps/api_usage_example_lib.exe apps/api_usage_example_lib.c libcab_game.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

word.o: src/core/word.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) cab_game libcab_game.a word.o $(CAB_GAME_LIB_OBJS) *.exe

help:
	@echo "make [target]"
	@echo "Targets: game, game-lib, debug, cab_game, libcab_game.a, word.o, clean"
