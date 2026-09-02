# Cows and Bulls in C

[![Language](https://img.shields.io/badge/language-C11-00599C.svg?logo=c&logoColor=white)](https://en.cppreference.com/w/c/11)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)](#requirements)
[![Build Tool](https://img.shields.io/badge/build-GNU%20Make-informational.svg?logo=gnu&logoColor=white)](#building-and-running)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A modular C11 implementation and core engine for the classic [Cows and Bulls](https://en.wikipedia.org/wiki/Bulls_and_cows) word guessing game.

The project is structured as a reusable core engine with a public API (`cab_api.h`) and multiple frontends / example applications in `apps/`.

## Game Overview

In **Cows and Bulls**, the player attempts to guess a secret target word (default: 5 letters) using clues provided after each guess:

- **Bull**: A correct letter in the correct position.
- **Cow**: A correct letter in the wrong position.

## Features

- **Decoupled Architecture**: Core game logic, I/O handling, command parsing, and session persistence are separated into isolated modules.
- **In-Game Helper Tools**:
  - `attempts`: View past guesses and scores, or validate candidate words against previous feedback.
  - `list`: Filter vocabulary using wildcard patterns (`a**de`), intersections (`-i`), and exclusions (`-r`).
- **Game State Persistence**: Save and resume active game sessions across runs.
- **Standalone API**: Easily embed the engine into other CLIs, GUIs, or test harnesses.
- **Customizable Vocabulary**: Support for arbitrary word length lists and language dictionaries.
- **Customizable Settings and Rules**: using the `set_setting()` function you can configure rules and settings

## Project Structure

```text
.
├── apps/               # Main CLI game, examples, and test runners
│   ├── api_usage_example.c
│   ├── test.c
│   └── utils/
├── data/
│   ├── saves/              # Game session save files
│   └── words/              # Dictionaries (e.g., 5_letters_en_words.txt)
├── src/
│   ├── api/                # Public API
│   ├── cmd/            # Command implementations (attempts, list, help, etc.)
│   ├── cmd_parser/         # User input parsing
│   ├── data_types/         # Internal data structures
│   ├── game/               # Core game rules, scoring, and filters
│   └── io/                 # File I/O, saves, and output formatting
└── Makefile                # Build configuration
```

## Requirements

- **C Compiler**: GCC or Clang (C11 standard support)
- **Build Tool**: GNU Make (cross-platform support for Linux, macOS, and Windows via MinGW/MSYS2)

## Building and Running

### Main Game

Build the default executable:
```bash
make game
```

Run the compiled executable:
- **Windows**: `cab_game.exe`
- **Linux / macOS**: `./cab_game`

Run directly with unoptimized debug flags (`-O0 -g`):
```bash
make debug
```

### Running Tests

Compile and execute the test suite:
```bash
make test
```

### Running Example Apps

Compile and run a specific app from the `apps/` directory:
```bash
make app f=simplest_example
make app f=test_mixed_letters_vocs
```

Compile without executing:
```bash
make build-app f=simplest_example
```

### Static Library

Build `libcab_game.a` for linking in external projects:
```bash
make game-lib
```

### Cleaning

```bash
make clean        # Remove binaries and intermediate object files
make distclean    # Clean everything including compile_commands.json
```

## In-Game Commands

When running the interactive game, the following commands are available at the prompt:

| Command | Description | Example |
| :--- | :--- | :--- |
| `<word>` | Guess a 5-letter word | `crane` |
| `attempts` | Display all past guesses and their scores | `attempts` |
| `attempts <word>` | Compare a candidate word against all past feedback | `attempts slate` |
| `list <pattern>` | Reset filter and find words matching pattern (`*` as wildcard) | `list a**de` or `list e` |
| `list -i <patterns>` | Narrow filter by intersecting with additional patterns | `list -i *r***` |
| `list -r <patterns>` | Exclude words matching pattern from current list | `list -r ****s` |
| `list -p` | Print current filtered candidate words | `list -p` |
| `list -h` | Print filter history | `list -h` |
| `surrender` | Concede the match and reveal the secret word | `surrender` |
| `help` | Print the command help menu | `help` |


## API Usage

You can embed the engine in custom applications via `cab_api.h`:

```c
#include <stdio.h>
#include <stdlib.h>
#include "cab_api.h"

int main(void) {
    cab_start_new_game();

    while (!cab_is_game_ended()) {
        char buffer[100];
        printf("%s", cab_get_input_prompt());

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        char* response = cab_play_turn(buffer);
        printf("%s", response);
        free(response);
    }

    cab_shutdown_game();
    return 0;
}
```

More advanced API features include
- choosing custom vocabulary and saves location
- tagged output, allowing to filter messages
- creating custom commands (currently a little hacky)
For more examples, see the [`apps/`](apps/) folder.


## License

This project is open source and available under the MIT License.
