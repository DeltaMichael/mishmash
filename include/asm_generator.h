#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "tac_generator.h"
#include "string_builder.h"
#include "hashmap.h"

typedef struct {
	LIST* quads;
	SYM_TABLE* sym_table;
	H_MAP* registers;
	STRING_BUILDER* out;
} ASM_GENERATOR;

ASM_GENERATOR* init_asm_generator(LIST* quads, SYM_TABLE* sym_table);
char* ag_alloc_reg(ASM_GENERATOR* asm_gen, char* var_name);
char* ag_realloc_reg(ASM_GENERATOR* asm_gen, char* reg);
char* ag_get_temp_reg(ASM_GENERATOR* asm_gen);
int ag_alloc_stack(ASM_GENERATOR* asm_gen);
void ag_free_reg(ASM_GENERATOR* asm_gen, char* reg);
void ag_index_variables(ASM_GENERATOR* asm_gen);
void ag_add_exit(ASM_GENERATOR* asm_gen);
void ag_generate_code(ASM_GENERATOR* asm_gen);
char* ag_get_code(ASM_GENERATOR* gen);

#endif

