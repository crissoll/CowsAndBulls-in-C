#ifndef CAB_INSTR
#define CAB_INSTR

#include <stdbool.h>

#include "utils.h"

#define MAX_ARGUMENTS_PER_INSTRUCTION 10

typedef struct {
    union{
        struct {
            char arguments[MAX_ARGUMENTS_PER_INSTRUCTION][MAX_ARGUMENT_SIZE];
        };
        char name[MAX_ARGUMENT_SIZE];
    };
    size_t args_count;
} Instruction;

typedef struct{
    char* name;
    size_t min_args_count;
    size_t max_args_count;
    void (*function_ptr)(Instruction*);
} InstructionDefiner;

bool instruction__init_from_input(Instruction* instr);

bool instruction__try(
    Instruction* instruction,
    InstructionDefiner* definer
);
#endif 