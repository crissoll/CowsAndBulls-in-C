#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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


bool check_arguments_bounds(size_t args,size_t min_count,size_t max_count){
    if (args > max_count){
        printf("too many arguments\n");
        return false;
    }
    if (args < min_count){
        printf("too few arguments\n");
        return false;
    }
    return true;
}

bool instruction__init_from_input(Instruction* instr){
    instr->args_count = get_multiple_input(instr->arguments, MAX_ARGUMENTS_PER_INSTRUCTION);
    
    if(instr->args_count==(size_t)-1){
        printf("too many arguments");
        return false;
    }
    if(instr->args_count == 0){
        return false;
    }
    return false;
}


bool instruction__try(
        Instruction* instruction,
        InstructionDefiner* definer
    ){

    if (strcmp(definer->name,instruction->name) != 0)
        return false;
    
    if(!check_arguments_bounds(
            instruction->args_count,
            definer->min_args_count,
            definer->max_args_count))
        return false;
    
    definer->function_ptr(instruction);
    return true;
}
