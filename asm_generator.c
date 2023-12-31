#include "include/asm_generator.h"
#include "include/asm_templates.h"
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
		ag_assign_quad(asm_gen, quad, index);
	} else if (strcmp(op, "+") == 0) {
		ag_add_quad(asm_gen, quad, index);
	} else if (strcmp(op, "-") == 0) {

	} else if (strcmp(op, "*") == 0) {
		ag_mul_quad(asm_gen, quad, index);
	} else if (strcmp(op, "/") == 0) {

	} else if (strcmp(op, "uminus") == 0) {
		ag_uminus_quad(asm_gen, quad, index);
	} else if (strcmp(op, "print") == 0) {
		ag_print_quad(asm_gen, quad, index);
	}
}

void ag_assign_quad(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	SYM_TABLE* table = asm_gen->sym_table;
	char* input = quad->arg1;
	char* result = quad->result;
	VAR_DATA* result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char* reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}
	// check if we're pushing a literal
	if(contains_key(table->variables, input)) {
		VAR_DATA* input_data = hashmap_get(table->variables, input);
		if (input_data->location == REGISTER && result_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, input_data->reg_name, result_data->reg_name);
		}
		if (input_data->location == STACK && result_data->location == REGISTER) {
			mov_stack_reg(asm_gen->out, input_data->offset, result_data->reg_name);
		}
		if (input_data->location == REGISTER && result_data->location == STACK) {
			mov_reg_stack(asm_gen->out, input_data->reg_name, result_data->offset);
		}
		if (input_data->location == STACK && result_data->location == STACK) {
			char* temp_reg = ag_get_temp_reg(asm_gen);
			mov_stack_stack(asm_gen->out, input_data->offset, result_data->offset, temp_reg);
		}
	} else {
		if (result_data->location == REGISTER) {
			mov_val_reg(asm_gen->out, input, result_data->reg_name);
		} else if (result_data->location == STACK) {
			mov_val_stack(asm_gen->out, input, result_data->offset);
		}
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, input, index);
}

void ag_add_quad(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	SYM_TABLE* table = asm_gen->sym_table;
	char* mfirst = quad->arg1;
	char* msecond = quad->arg2;
	char* result = quad->result;
	VAR_DATA* result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char* reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	char* temp_reg;
	// If the result variable is a register, use it for the result
	// If it's on the stack, use a temp register for the result
	if (result_data->location == REGISTER) {
		temp_reg = result_data->reg_name;
	} else {
		char* temp_reg = ag_get_temp_reg(asm_gen);
	}
	// check if both factors are variables
	if(contains_key(table->variables, mfirst) && contains_key(table->variables, msecond)) {
		VAR_DATA* mfirst_data = hashmap_get(table->variables, mfirst);
		VAR_DATA* msecond_data = hashmap_get(table->variables, msecond);
		if (mfirst_data->location == REGISTER && msecond_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
			add_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK && msecond_data->location == REGISTER) {
			mov_stack_reg(asm_gen->out, mfirst_data->offset, temp_reg);
			add_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == REGISTER && msecond_data->location == STACK) {
			mov_stack_reg(asm_gen->out, msecond_data->offset, temp_reg);
			add_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK && msecond_data->location == STACK) {
			add_stack_stack(asm_gen->out, mfirst_data->offset, msecond_data->offset, temp_reg);
		}
	} else {
		// first is variable, second is literal
		if (contains_key(table->variables, mfirst)) {
			VAR_DATA* mfirst_data = hashmap_get(table->variables, mfirst);
			if(mfirst_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
				add_val_reg(asm_gen->out, msecond, temp_reg);
			}
			if(mfirst_data->location == STACK) {
				mov_stack_reg(asm_gen->out, mfirst_data->offset, temp_reg);
				add_val_reg(asm_gen->out, msecond, temp_reg);
			}
		// first is literal, second is variable
		} else if (contains_key(table->variables, msecond)) {
			VAR_DATA* msecond_data = hashmap_get(table->variables, msecond);
			if(msecond_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
				add_val_reg(asm_gen->out, mfirst, temp_reg);
			}
			if(msecond_data->location == STACK) {
				mov_stack_reg(asm_gen->out, msecond_data->offset, temp_reg);
				add_val_reg(asm_gen->out, mfirst, temp_reg);
			}
		} else {
			// TODO: What if they're both literals?
			printf("IMPLEMENT ADDITION OF TWO LITERALS");
		}
	}
	if (result_data->location == STACK) {
		mov_reg_stack(asm_gen->out, temp_reg, result_data->offset);
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, mfirst, index);
	ag_try_free_variable(asm_gen, msecond, index);
}

void ag_mul_quad(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	SYM_TABLE* table = asm_gen->sym_table;
	char* mfirst = quad->arg1;
	char* msecond = quad->arg2;
	char* result = quad->result;
	VAR_DATA* result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char* reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	char* temp_reg;
	// If the result variable is a register, use it for the result
	// If it's on the stack, use a temp register for the result
	if (result_data->location == REGISTER) {
		temp_reg = result_data->reg_name;
	} else {
		char* temp_reg = ag_get_temp_reg(asm_gen);
	}
	// check if both factors are variables
	if(contains_key(table->variables, mfirst) && contains_key(table->variables, msecond)) {
		VAR_DATA* mfirst_data = hashmap_get(table->variables, mfirst);
		VAR_DATA* msecond_data = hashmap_get(table->variables, msecond);
		if (mfirst_data->location == REGISTER && msecond_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
			mul_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK && msecond_data->location == REGISTER) {
			mov_stack_reg(asm_gen->out, mfirst_data->offset, temp_reg);
			mul_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == REGISTER && msecond_data->location == STACK) {
			mov_stack_reg(asm_gen->out, msecond_data->offset, temp_reg);
			mul_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK && msecond_data->location == STACK) {
			mul_stack_stack(asm_gen->out, mfirst_data->offset, msecond_data->offset, temp_reg);
		}
	} else {
		// first is variable, second is literal
		if (contains_key(table->variables, mfirst)) {
			VAR_DATA* mfirst_data = hashmap_get(table->variables, mfirst);
			if(mfirst_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, mfirst_data->reg_name, temp_reg);
				mul_val_reg(asm_gen->out, msecond, temp_reg);
			}
			if(mfirst_data->location == STACK) {
				mov_stack_reg(asm_gen->out, mfirst_data->offset, temp_reg);
				mul_val_reg(asm_gen->out, msecond, temp_reg);
			}
		// first is literal, second is variable
		} else if (contains_key(table->variables, msecond)) {
			VAR_DATA* msecond_data = hashmap_get(table->variables, msecond);
			if(msecond_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, msecond_data->reg_name, temp_reg);
				mul_val_reg(asm_gen->out, mfirst, temp_reg);
			}
			if(msecond_data->location == STACK) {
				mov_stack_reg(asm_gen->out, msecond_data->offset, temp_reg);
				mul_val_reg(asm_gen->out, mfirst, temp_reg);
			}
		} else {
			// TODO: What if they're both literals?
			printf("IMPLEMENT MULTIPLICATION OF TWO LITERALS");
		}
	}
	if (result_data->location == STACK) {
		mov_reg_stack(asm_gen->out, temp_reg, result_data->offset);
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, mfirst, index);
	ag_try_free_variable(asm_gen, msecond, index);
}

void ag_uminus_quad(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	SYM_TABLE* table = asm_gen->sym_table;
	char* input = quad->arg1;
	char* result = quad->result;
	VAR_DATA* result_data = hashmap_get(table->variables, result);


	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char* reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	if(contains_key(table->variables, input)) {
		VAR_DATA* input_data = hashmap_get(table->variables, input);
		if (input_data->location == REGISTER && result_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, input_data->reg_name, result_data->reg_name);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (input_data->location == STACK && result_data->location == REGISTER) {
			mov_stack_reg(asm_gen->out, input_data->offset, result_data->reg_name);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (input_data->location == REGISTER && result_data->location == STACK) {
			mov_reg_stack(asm_gen->out, input_data->reg_name, result_data->offset);
			char* temp_reg = ag_get_temp_reg(asm_gen);
			neg_stack(asm_gen->out, result_data->offset, temp_reg);
		}
		if (input_data->location == STACK && result_data->location == STACK) {
			char* temp_reg = ag_get_temp_reg(asm_gen);
			mov_stack_reg(asm_gen->out, input_data->offset, temp_reg);
			neg_reg(asm_gen->out, temp_reg);
			mov_reg_stack(asm_gen->out, temp_reg, result_data->offset);
		}
	} else {
		if (result_data->location == REGISTER) {
			mov_val_reg(asm_gen->out, input, result_data->reg_name);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (result_data->location == STACK) {
			char* temp_reg = ag_get_temp_reg(asm_gen);
			mov_val_stack(asm_gen->out, input, result_data->offset);
			neg_stack(asm_gen->out, result_data->offset, temp_reg);
		}
	}
	ag_try_free_variable(asm_gen, input, index);
}

void ag_print_quad(ASM_GENERATOR* asm_gen, QUAD* quad, int index) {
	ag_preserve_registers(asm_gen);
	SYM_TABLE* table = asm_gen->sym_table;
	char* input = quad->arg1;
	// print a variable
	if(contains_key(table->variables, input)) {
		VAR_DATA* input_data = hashmap_get(table->variables, input);
		if (input_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, input_data->reg_name, "rdi");
		}
		if (input_data->location == STACK) {
			mov_stack_reg(asm_gen->out, input_data->offset, "rdi");
		}
	// print a literal
	} else {
		mov_val_reg(asm_gen->out, input, "rdi");
	}
	call(asm_gen->out, "print_num");
	ag_restore_registers(asm_gen);
}

void ag_preserve_registers(ASM_GENERATOR* asm_gen) {
	for (int i = 0; i < REGCOUNT; i++) {
		if(hashmap_get(asm_gen->registers, regnames[i]) != NULL) {
			push_reg(asm_gen->out, regnames[i]);
		}
	}
}

void ag_restore_registers(ASM_GENERATOR* asm_gen) {
	for (int i = REGCOUNT - 1; i >= 0; i--) {
		if(hashmap_get(asm_gen->registers, regnames[i]) != NULL) {
			pop_reg(asm_gen->out, regnames[i]);
		}
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
	// TODO: Move variable to stack if no free register
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

void ag_try_free_variable(ASM_GENERATOR* asm_gen, char* var, int index) {
	SYM_TABLE* table = asm_gen->sym_table;
	VAR_DATA* var_data = hashmap_get(table->variables, var);
	if(contains_key(table->variables, var)) {
		if (var_data->location == REGISTER && var_data->last_index == index) {
			ag_free_reg(asm_gen, var_data->reg_name);
		}
	}
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
	LIST* quads = asm_gen->quads;
	for(int i = 0; i < quads->size; i++) {
		QUAD* quad = list_get(asm_gen->quads, i);
		ag_quad_to_asm(asm_gen, quad, i);
	}
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

