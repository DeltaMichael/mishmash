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
char* ag_get_code(ASM_GENERATOR* gen);

#endif

