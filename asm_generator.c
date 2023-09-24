#include "include/asm_generator.h"
#include "include/hashmap.h"
#include "include/string_builder.h"
#include <stdio.h>
#include <string.h>

ASM_GENERATOR* init_asm_generator(LIST* quads, SYM_TABLE* sym_table) {
	ASM_GENERATOR* gen = malloc(sizeof(ASM_GENERATOR));
	gen->quads = quads;
	gen->sym_table = sym_table;
	gen->registers = init_hashmap();
	hashmap_put(gen->registers, "rax", NULL);
	hashmap_put(gen->registers, "rbx", NULL);
	hashmap_put(gen->registers, "rcx", NULL);
	hashmap_put(gen->registers, "rdx", NULL);
	hashmap_put(gen->registers, "rdi", NULL);
	hashmap_put(gen->registers, "rsi", NULL);
	hashmap_put(gen->registers, "rdi", NULL);
	hashmap_put(gen->registers, "r8", NULL);
	hashmap_put(gen->registers, "r9", NULL);
	hashmap_put(gen->registers, "r10", NULL);
	hashmap_put(gen->registers, "r11", NULL);
	hashmap_put(gen->registers, "r12", NULL);
	hashmap_put(gen->registers, "r13", NULL);
	hashmap_put(gen->registers, "r14", NULL);
	hashmap_put(gen->registers, "r15", NULL);
	gen->out = init_sb();
	sb_append(gen->out, ".globl _start\n");
	sb_append(gen->out, ".section .text\n");
	sb_append(gen->out, "_start:\n");

	// Stack allocate non-temp variables
	sb_append(gen->out, "\tenter ");
	char buf[20];
	sprintf(buf, "%d", gen->sym_table->offset);
	sb_append(gen->out, buf);
	sb_append(gen->out, ", $0\n");

	return gen;
}

void ag_index_variables(ASM_GENERATOR* asm_gen) {
	LIST* quads = asm_gen->quads;
	SYM_TABLE* table = asm_gen->sym_table;
	for(int i = quads->size - 1; i >= 0; i--) {
		QUAD* quad = list_get(quads, i);
		char* vars[3];
		vars[0] = quad->arg1;
		vars[1] = quad->arg2;
		vars[2] = quad->result;
		for(int j = 0; j < 3; j++) {
			if(vars[j] != NULL && contains_key(table->variables, vars[j])) {
				VAR_DATA* data = hashmap_get(table->variables, vars[j]);
				if (data->last_index == -1) data->last_index = i;
				data->first_index = i;
			}
		}
	}
}

void ag_quad_to_asm(ASM_GENERATOR* asm_gen, QUAD* quad) {
	char* op = quad->op;
	if(strcmp(op, ":=") == 0) {

	} else if (strcmp(op, "+") == 0) {

	} else if (strcmp(op, "-") == 0) {

	} else if (strcmp(op, "*") == 0) {

	} else if (strcmp(op, "/") == 0) {

	}
}

void ag_generate_code(ASM_GENERATOR* asm_gen) {
	ag_index_variables(asm_gen);
}

char* ag_get_code(ASM_GENERATOR* gen) {
	char* out = sb_build(gen->out);
	return out;
}

