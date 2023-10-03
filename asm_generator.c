#include "include/asm_generator.h"
#include "include/hashmap.h"
#include "include/string_builder.h"
#include <stdio.h>
#include <string.h>
#define REGCOUNT 14
char* regnames[REGCOUNT] = { "rax", "rbx", "rcx", "rdx", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

ASM_GENERATOR* init_asm_generator(LIST* quads, SYM_TABLE* sym_table) {
	ASM_GENERATOR* gen = malloc(sizeof(ASM_GENERATOR));
	gen->quads = quads;
	gen->sym_table = sym_table;
	gen->registers = init_hashmap();
	for (int i = 0; i < REGCOUNT; i++) {
		hashmap_put(gen->registers, regnames[i], NULL);
	}
	gen->out = init_sb();
	sb_append(gen->out, ".globl _start\n");
	sb_append(gen->out, ".section .text\n");
	sb_append(gen->out, "_start:\n");

	// Stack allocate non-temp variables
	sb_append(gen->out, "\tenter $");
	sb_append_int(gen->out, gen->sym_table->offset);
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

void ag_quad_to_asm(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	char* op = quad->op;
	SYM_TABLE* table = asm_gen->sym_table;
	if(strcmp(op, ":=") == 0) {
		char* value = quad->arg1;
		char* var = quad->result;
		VAR_DATA* var_data = hashmap_get(table->variables, var);
		if(contains_key(table->variables, value)) {
			VAR_DATA* value_data = hashmap_get(table->variables, value);
			if (value_data->location == REGISTER) {
				sb_append(asm_gen->out, "\tmovq %");
				sb_append(asm_gen->out, value_data->reg_name);
				if (value_data->last_index == index) {
					ag_free_reg(asm_gen, value_data->reg_name);
				}
			} else if (value_data->location == STACK) {
				char* temp_reg = ag_get_temp_reg(asm_gen);
				sb_append(asm_gen->out, "\tmovq -");
				sb_append_int(asm_gen->out, value_data->offset);
				sb_append(asm_gen->out, "(%rbp), %");
				sb_append(asm_gen->out, temp_reg);
				sb_append(asm_gen->out, "\n");
				sb_append(asm_gen->out, "\tmovq %");
				sb_append(asm_gen->out, temp_reg);
			}
		} else {
			sb_append(asm_gen->out, "\tmovq $");
			sb_append(asm_gen->out, value);
		}
		if (var_data->location == REGISTER) {
			char* reg_name = ag_alloc_reg(asm_gen, var);
			sb_append(asm_gen->out, ", %");
			sb_append(asm_gen->out, reg_name);
		} else if (var_data->location == STACK) {
			sb_append(asm_gen->out, ", -");
			sb_append_int(asm_gen->out, var_data->offset);
			sb_append(asm_gen->out, "(%rbp)\n");
		}
		// create assignment code
		// deallocate register variables that can be deallocated
	} else if (strcmp(op, "+") == 0) {

	} else if (strcmp(op, "-") == 0) {

	} else if (strcmp(op, "*") == 0) {

	} else if (strcmp(op, "/") == 0) {

	} else if (strcmp(op, "uminus") == 0) {

	}
}

char* ag_alloc_reg(ASM_GENERATOR* asm_gen, char* var) {
	SYM_TABLE* table = asm_gen->sym_table;
	VAR_DATA* data = hashmap_get(table->variables, var);
	if (data->location != REGISTER || data->reg_name) {
		return NULL;
	}
	for (int i = 0; i < REGCOUNT; i++) {
		if(hashmap_get(asm_gen->registers, regnames[i]) == NULL) {
			hashmap_put(asm_gen->registers, regnames[i], var);
			data->reg_name = regnames[i];
			return regnames[i];
		}
	}
	// TODO: Allocate on stack if this returns NULL
	return NULL;
}

char* ag_get_temp_reg(ASM_GENERATOR* asm_gen) {
	for (int i = 0; i < REGCOUNT; i++) {
		if (hashmap_get(asm_gen->registers, regnames[i]) == NULL) {
			return regnames[i];
		}
	}
	return NULL;
}

char* ag_realloc_reg(ASM_GENERATOR* asm_gen, char* reg) {
	char* var_name = hashmap_get(asm_gen->registers, reg);
	if (var_name == NULL) {
		return reg;
	}
	char* reg_name = ag_alloc_reg(asm_gen, var_name);
	ag_free_reg(asm_gen, reg);
	return reg_name;
}

void ag_free_reg(ASM_GENERATOR* asm_gen, char* reg) {
	SYM_TABLE* table = asm_gen->sym_table;
	char* var_name = hashmap_get(asm_gen->registers, reg);
	hashmap_put(asm_gen->registers, reg, NULL);
	VAR_DATA* data = hashmap_get(table->variables, var_name);
	data->reg_name = NULL;
}

void ag_generate_code(ASM_GENERATOR* asm_gen) {
	ag_index_variables(asm_gen);
	QUAD* quad = list_get(asm_gen->quads, 0);
	ag_quad_to_asm(asm_gen, quad, 0);
	quad = list_get(asm_gen->quads, 1);
	ag_quad_to_asm(asm_gen, quad, 1);
	quad = list_get(asm_gen->quads, 2);
	ag_quad_to_asm(asm_gen, quad, 2);
	ag_add_exit(asm_gen);
}

void ag_add_exit(ASM_GENERATOR* asm_gen) {
	sb_append(asm_gen->out, "\tleave\n");
	sb_append(asm_gen->out, "\tmovq $0, %rdi\n");
	sb_append(asm_gen->out, "\tmovq $60, %rax\n");
	sb_append(asm_gen->out, "\tsyscall\n");
}

char* ag_get_code(ASM_GENERATOR* gen) {
	char* out = sb_build(gen->out);
	return out;
}

