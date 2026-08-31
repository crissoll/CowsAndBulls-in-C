# Suppress command echoing for clean output
.SILENT:

# ==============================================================================
# 1. PLATFORM & TOOLCHAIN CONFIGURATION
# ==============================================================================
ifeq ($(OS),Windows_NT)
    RM       := del /F /Q
    EXE      := .exe
    NULL     := NUL
    WHICH    := where
    FIXPATH   = $(subst /,\,$1)
    RUN_CMD   = $(call FIXPATH,$1)
else
    RM       := rm -f
    EXE      :=
    NULL     := /dev/null
    WHICH    := which
    FIXPATH   = $1
    RUN_CMD   = ./$1
endif

# Auto-detect compiler: prefer clang -> fallback to gcc -> fallback to cc
ifeq ($(origin CC),default)
    ifneq ($(shell $(WHICH) clang 2>$(NULL)),)
        CC := clang
    else ifneq ($(shell $(WHICH) gcc 2>$(NULL)),)
        CC := gcc
    else
        CC ?= cc
    endif
endif

AR := ar rcs

# ==============================================================================
# 2. SOURCES, INCLUDES & FLAGS
# ==============================================================================
# Recursive wildcard helper
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)$(filter $(subst *,%,$2),$d))

MAIN_SRC      := apps/api_usage_example.c
APP_UTIL_SRCS := $(call rwildcard,apps/utils/,*.c)
LIB_SRCS      := $(call rwildcard,src/,*.c)
ALL_SRCS      := $(MAIN_SRC) $(LIB_SRCS)
LIB_OBJS      := $(LIB_SRCS:.c=.o)

SRC_DIRS := $(sort $(dir $(ALL_SRCS)))
INCLUDES := $(addprefix -I,$(SRC_DIRS))
CFLAGS   := -g -Wall -Wextra -std=c11 $(INCLUDES)

COMPILE_COMMANDS := compile_commands.json

# ==============================================================================
# 3. BUILD TARGETS
# ==============================================================================
.PHONY: all game game-lib debug app test clean clean-objs distclean rebuild help

# Clean up intermediate objects automatically
.INTERMEDIATE: $(LIB_OBJS)

all: cab_game$(EXE) | $(COMPILE_COMMANDS)

game: cab_game$(EXE)

game-lib: libcab_game.a

# Main game executable
cab_game$(EXE): $(ALL_SRCS) | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -o $@ $^

# Static library build
libcab_game.a: $(LIB_OBJS) | $(COMPILE_COMMANDS)
	$(AR) $@ $^

%.o: %.c | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -c $< -o $@

# Example linked against static library
api_example_lib: libcab_game.a | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -o apps/api_usage_example_lib$(EXE) apps/api_usage_example_lib.c libcab_game.a

# ==============================================================================
# 4. EXECUTION & TESTING TARGETS
# ==============================================================================
# Run game with zero optimization for debugging
debug: CFLAGS += -O0
debug: cab_game$(EXE)
	$(call RUN_CMD,cab_game$(EXE))

# Run the test suite
test: CFLAGS += -O0
test: | $(COMPILE_COMMANDS)
	$(CC) $(CFLAGS) -o apps/test$(EXE) apps/test.c $(APP_UTIL_SRCS) $(LIB_SRCS)
	$(call RUN_CMD,apps/test$(EXE))

# Compile and run any app from apps/ (e.g., make app f=simplest_example)
app: CFLAGS += -O0
app: | $(COMPILE_COMMANDS)
	$(if $(f),,$(error Error: Specify the app with f=<name>, e.g., 'make app f=simplest_example'))
	$(CC) $(CFLAGS) -o apps/$(f)$(EXE) apps/$(f).c $(APP_UTIL_SRCS) $(LIB_SRCS)
	$(call RUN_CMD,apps/$(f)$(EXE))

# ==============================================================================
# 5. LANGUAGE SERVER (compile_commands.json)
# ==============================================================================
comma := ,
json_escape = $(subst \,\\,$(subst ",\",$(1)))

define json_entry
{ \
  "directory": "$(call json_escape,$(CURDIR))", \
  "command": "$(call json_escape,$(CC) $(CFLAGS) -c $(1) -o $(basename $(1)).o)", \
  "file": "$(call json_escape,$(abspath $(1)))" \
}
endef

$(COMPILE_COMMANDS): $(ALL_SRCS)
	$(file >$@,[)
	$(foreach src,$(ALL_SRCS),\
		$(file >>$@,$(call json_entry,$(src)))\
		$(if $(filter $(src),$(lastword $(ALL_SRCS))),,\
			$(file >>$@,$(comma))\
		)\
	)
	$(file >>$@,])

# ==============================================================================
# 6. CLEANING & HELP
# ==============================================================================
clean-objs:
	-$(RM) $(call FIXPATH,$(LIB_OBJS)) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,apps/*.o) >$(NULL) 2>&1

clean: clean-objs
	-$(RM) $(call FIXPATH,cab_game$(EXE)) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,libcab_game.a) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,*.exe) >$(NULL) 2>&1
	-$(RM) $(call FIXPATH,apps/*.exe) >$(NULL) 2>&1

distclean: clean
	-$(RM) $(COMPILE_COMMANDS) >$(NULL) 2>&1

rebuild: clean all

help:
	echo "Usage: make [target]"
	echo ""
	echo "Available Targets:"
	echo "  all        - Build main executable and compile database (default)"
	echo "  game       - Build main game executable (cab_game)"
	echo "  game-lib   - Build static library (libcab_game.a)"
	echo "  debug      - Build with -O0 and run the game"
	echo "  test       - Build and run the test suite (apps/test.c)"
	echo "  app        - Build and run a specific app (e.g. make app f=simplest_example)"
	echo "  clean      - Remove compiled binaries and object files"
	echo "  distclean  - Clean everything including compile_commands.json"
	echo "  rebuild    - Full clean and rebuild"