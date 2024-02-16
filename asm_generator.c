#include "include/asm_generator.h"
#include "include/asm_templates.h"
#include "include/hashmap.h"
#include "include/string_builder.h"
#include <stdio.h>
#include <string.h>

#define REGCOUNT 14

char *regnames[REGCOUNT] =
    { "rax", "rbx", "rcx", "rdx", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12",
	"r13", "r14", "r15"
};

char *regnames_byte[REGCOUNT] =
    { "al", "bl", "cl", "dl", "sil", NULL, "r8b", "r9b", "r10b", "r11b", "r12b",
	"r13b", "r14b", "r15b"
};

ASM_GENERATOR *init_asm_generator(LIST *quads, SYM_TABLE *sym_table)
{
	ASM_GENERATOR *gen = malloc(sizeof(ASM_GENERATOR));
	gen->quads = quads;
	gen->sym_table = sym_table;
	gen->registers = init_hashmap();
	for (int i = 0; i < REGCOUNT; i++) {
		hashmap_put(gen->registers, regnames[i], NULL);
	}
	gen->out = init_sb();
	// TODO: Find way to auto-generate function defs
	sb_append(gen->out, "extern print_num\n");
	sb_append(gen->out, "global\t_start\n");
	sb_append(gen->out, "section\t.text\n");
	sb_append(gen->out, "_start:\n");

	// Stack allocate non-temp variables
	sb_append(gen->out, "\tenter ");
	sb_append_int(gen->out, gen->sym_table->offset);
	sb_append(gen->out, ", 0\n");

	return gen;
}

void free_asm_generator(ASM_GENERATOR *asm_gen)
{
	free_sb(asm_gen->out);
	asm_gen->out = NULL;
	free_hashmap(asm_gen->registers);
	asm_gen->registers = NULL;
	free_symtable(asm_gen->sym_table);
	asm_gen->sym_table = NULL;
	free_list(asm_gen->quads, free_quad);
	asm_gen->quads = NULL;
	free(asm_gen);
	asm_gen = NULL;
}

void ag_index_variables(ASM_GENERATOR *asm_gen)
{
	LIST *quads = asm_gen->quads;
	SYM_TABLE *table = asm_gen->sym_table;
	for (int i = quads->size - 1; i >= 0; i--) {
		QUAD *quad = list_get(quads, i);
		char *vars[3];
		vars[0] = quad->arg1;
		vars[1] = quad->arg2;
		vars[2] = quad->result;
		for (int j = 0; j < 3; j++) {
			if (vars[j] != NULL
			    && contains_key(table->variables, vars[j])) {
				VAR_DATA *data =
				    hashmap_get(table->variables, vars[j]);
				if (data->last_index == -1)
					data->last_index = i;
				data->first_index = i;
			}
		}
	}
}

void ag_quad_to_asm(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	char *op = quad->op;
	SYM_TABLE *table = asm_gen->sym_table;
	if (strcmp(op, ":=") == 0) {
		ag_assign_quad(asm_gen, quad, index);
	} else if (strcmp(op, "+") == 0) {
		ag_add_quad(asm_gen, quad, index);
	} else if (strcmp(op, "-") == 0) {
		ag_sub_quad(asm_gen, quad, index);
	} else if (strcmp(op, "*") == 0) {
		ag_mul_quad(asm_gen, quad, index);
	} else if (strcmp(op, "/") == 0) {
	} else if (strcmp(op, "uminus") == 0) {
		ag_uminus_quad(asm_gen, quad, index);
	} else if (strcmp(op, "print") == 0) {
		ag_print_quad(asm_gen, quad, index);
	} else if (strcmp(op, "=") == 0 || strcmp(op, "<") == 0
		   || strcmp(op, ">") == 0 || strcmp(op, "<=") == 0
		   || strcmp(op, ">=") == 0) {
		ag_comparison_quad(asm_gen, quad, index);
	}
}

void ag_assign_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *input = quad->arg1;
	char *result = quad->result;
	VAR_DATA *result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char *reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}
	// check if we're pushing a literal
	if (contains_key(table->variables, input)) {
		VAR_DATA *input_data = hashmap_get(table->variables, input);

		if (input_data->location == REGISTER
		    && result_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, result_data->reg_name, input_data->reg_name);
		}
		if (input_data->location == STACK
		    && result_data->location == REGISTER) {
			mov_reg_stack(asm_gen->out, result_data->reg_name, input_data->offset);
		}
		if (input_data->location == REGISTER
		    && result_data->location == STACK) {
			mov_stack_reg(asm_gen->out, result_data->offset, input_data->reg_name);
		}
		if (input_data->location == STACK
		    && result_data->location == STACK) {
			char *temp_reg = ag_get_temp_reg(asm_gen);
			mov_stack_stack(asm_gen->out, result_data->offset, input_data->offset, temp_reg);
		}
	} else {
		if (result_data->location == REGISTER) {
			mov_reg_val(asm_gen->out, result_data->reg_name, input);
		} else if (result_data->location == STACK) {
			mov_stack_val(asm_gen->out, result_data->offset, input);
		}
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, input, index);
}



void op_template(ASM_GENERATOR *asm_gen, QUAD *quad, int index, RRO reg_reg_op, SSO stack_stack_op, VRO val_reg_op)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *mfirst = quad->arg1;
	char *msecond = quad->arg2;
	char *result = quad->result;
	VAR_DATA *result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char *reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	char *temp_reg;
	// If the result variable is a register, use it for the result
	// If it's on the stack, use a temp register for the result
	if (result_data->location == REGISTER) {
		temp_reg = result_data->reg_name;
	} else {
		char *temp_reg = ag_get_temp_reg(asm_gen);
	}
	// check if both factors are variables
	if (contains_key(table->variables, mfirst)
	    && contains_key(table->variables, msecond)) {
		VAR_DATA *mfirst_data = hashmap_get(table->variables, mfirst);
		VAR_DATA *msecond_data = hashmap_get(table->variables, msecond);
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, temp_reg, mfirst_data->reg_name);
			reg_reg_op(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == REGISTER) {
			mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
			reg_reg_op(asm_gen->out, msecond_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == STACK) {
			mov_reg_stack(asm_gen->out, temp_reg, msecond_data->offset);
			reg_reg_op(asm_gen->out, mfirst_data->reg_name, temp_reg);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == STACK) {
			stack_stack_op(asm_gen->out, mfirst_data->offset, msecond_data->offset, temp_reg);
		}
	} else {
		// first is variable, second is literal
		if (contains_key(table->variables, mfirst)) {
			VAR_DATA *mfirst_data =
			    hashmap_get(table->variables, mfirst);
			if (mfirst_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, temp_reg, mfirst_data->reg_name);
				val_reg_op(asm_gen->out, msecond, temp_reg);
			}
			if (mfirst_data->location == STACK) {
				mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
				val_reg_op(asm_gen->out, msecond, temp_reg);
			}
			// first is literal, second is variable
		} else if (contains_key(table->variables, msecond)) {
			VAR_DATA *msecond_data =
			    hashmap_get(table->variables, msecond);
			if (msecond_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, temp_reg, msecond_data->reg_name);
				val_reg_op(asm_gen->out, mfirst, temp_reg);
			}
			if (msecond_data->location == STACK) {
				mov_reg_stack(asm_gen->out, temp_reg, msecond_data->offset);
				val_reg_op(asm_gen->out, mfirst, temp_reg);
			}
		} else {
			mov_reg_val(asm_gen->out, temp_reg, mfirst);
			val_reg_op(asm_gen->out, msecond, temp_reg);
		}
	}
	if (result_data->location == STACK) {
		mov_stack_reg(asm_gen->out, result_data->offset, temp_reg);
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, mfirst, index);
	ag_try_free_variable(asm_gen, msecond, index);
}

void ag_add_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	op_template(asm_gen, quad, index, add_reg_reg, add_stack_stack, add_val_reg);
}

void ag_mul_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	op_template(asm_gen, quad, index, mul_reg_reg, mul_stack_stack, mul_val_reg);
}

void ag_sub_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *mfirst = quad->arg1;
	char *msecond = quad->arg2;
	char *result = quad->result;
	VAR_DATA *result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char *reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	char *temp_reg;
	// If the result variable is a register, use it for the result
	// If it's on the stack, use a temp register for the result
	if (result_data->location == REGISTER) {
		temp_reg = result_data->reg_name;
	} else {
		char *temp_reg = ag_get_temp_reg(asm_gen);
	}
	// check if both factors are variables
	if (contains_key(table->variables, mfirst)
	    && contains_key(table->variables, msecond)) {
		VAR_DATA *mfirst_data = hashmap_get(table->variables, mfirst);
		VAR_DATA *msecond_data = hashmap_get(table->variables, msecond);
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, temp_reg, mfirst_data->reg_name);
			sub_reg_reg(asm_gen->out, temp_reg, msecond_data->reg_name);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == REGISTER) {
			mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
			sub_reg_reg(asm_gen->out, temp_reg, msecond_data->reg_name);
		}
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == STACK) {
			mov_reg_reg(asm_gen->out, temp_reg, mfirst_data->reg_name);
			sub_reg_stack(asm_gen->out, temp_reg, msecond_data->offset);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == STACK) {
			mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
			sub_reg_stack(asm_gen->out, temp_reg, msecond_data->offset);
		}
	} else {
		// first is variable, second is literal
		if (contains_key(table->variables, mfirst)) {
			VAR_DATA *mfirst_data =
			    hashmap_get(table->variables, mfirst);
			if (mfirst_data->location == REGISTER) {
				mov_reg_reg(asm_gen->out, temp_reg, mfirst_data->reg_name);
				sub_reg_val(asm_gen->out, temp_reg, msecond);
			}
			if (mfirst_data->location == STACK) {
				mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
				sub_reg_val(asm_gen->out, temp_reg, msecond);
			}
			// first is literal, second is variable
		} else if (contains_key(table->variables, msecond)) {
			VAR_DATA *msecond_data =
			    hashmap_get(table->variables, msecond);
			if (msecond_data->location == REGISTER) {
				mov_reg_val(asm_gen->out, temp_reg, mfirst);
				sub_reg_reg(asm_gen->out, temp_reg, msecond_data->reg_name);
			}
			if (msecond_data->location == STACK) {
				mov_reg_val(asm_gen->out, temp_reg, mfirst);
				sub_reg_stack(asm_gen->out, temp_reg, msecond_data->offset);
			}
		} else {
			mov_reg_val(asm_gen->out, temp_reg, mfirst);
			sub_reg_val(asm_gen->out, temp_reg, msecond);
		}
	}
	if (result_data->location == STACK) {
		mov_stack_reg(asm_gen->out, result_data->offset, temp_reg);
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, mfirst, index);
	ag_try_free_variable(asm_gen, msecond, index);
}

void ag_uminus_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *input = quad->arg1;
	char *result = quad->result;
	VAR_DATA *result_data = hashmap_get(table->variables, result);

	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char *reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	if (contains_key(table->variables, input)) {
		VAR_DATA *input_data = hashmap_get(table->variables, input);
		if (input_data->location == REGISTER
		    && result_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, result_data->reg_name, input_data->reg_name);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (input_data->location == STACK
		    && result_data->location == REGISTER) {
			mov_reg_stack(asm_gen->out, result_data->reg_name, input_data->offset);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (input_data->location == REGISTER
		    && result_data->location == STACK) {
			mov_stack_reg(asm_gen->out, result_data->offset, input_data->reg_name);
			char *temp_reg = ag_get_temp_reg(asm_gen);
			neg_stack(asm_gen->out, result_data->offset, temp_reg);
		}
		if (input_data->location == STACK
		    && result_data->location == STACK) {
			char *temp_reg = ag_get_temp_reg(asm_gen);
			mov_reg_stack(asm_gen->out, temp_reg, input_data->offset);
			neg_reg(asm_gen->out, temp_reg);
			mov_stack_reg(asm_gen->out, result_data->offset, temp_reg);
		}
	} else {
		if (result_data->location == REGISTER) {
			mov_reg_val(asm_gen->out, result_data->reg_name, input);
			neg_reg(asm_gen->out, result_data->reg_name);
		}
		if (result_data->location == STACK) {
			char *temp_reg = ag_get_temp_reg(asm_gen);
			mov_stack_val(asm_gen->out, result_data->offset, input);
			neg_stack(asm_gen->out, result_data->offset, temp_reg);
		}
	}
	ag_try_free_variable(asm_gen, input, index);
}

void ag_print_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	ag_preserve_registers(asm_gen);
	SYM_TABLE *table = asm_gen->sym_table;
	char *input = quad->arg1;
	// print a variable
	if (contains_key(table->variables, input)) {
		VAR_DATA *input_data = hashmap_get(table->variables, input);
		if (input_data->location == REGISTER) {
			mov_reg_reg(asm_gen->out, "rdi", input_data->reg_name);
		}
		if (input_data->location == STACK) {
			mov_reg_stack(asm_gen->out, "rdi", input_data->offset);
		}
		// print a literal
	} else {
		mov_reg_val(asm_gen->out, "rdi", input);
	}
	call(asm_gen->out, "print_num");
	ag_restore_registers(asm_gen);
}

void ag_comparison_quad(ASM_GENERATOR *asm_gen, QUAD *quad, int index)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *mfirst = quad->arg1;
	char *msecond = quad->arg2;
	char *result = quad->result;
	VAR_DATA *result_data = hashmap_get(table->variables, result);
	// allocate register if not allocated already
	if (result_data->location == REGISTER && result_data->reg_name == NULL) {
		char *reg_name = ag_alloc_reg(asm_gen, result);
		result_data->reg_name = reg_name;
	}

	char *temp_reg;
	// If the result variable is a register, use it for the result
	// If it's on the stack, use a temp register for the result
	if (result_data->location == REGISTER) {
		temp_reg = result_data->reg_name;
	} else {
		char *temp_reg = ag_get_temp_reg(asm_gen);
	}
	// check if both factors are variables
	if (contains_key(table->variables, mfirst)
	    && contains_key(table->variables, msecond)) {
		VAR_DATA *mfirst_data = hashmap_get(table->variables, mfirst);
		VAR_DATA *msecond_data = hashmap_get(table->variables, msecond);
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == REGISTER) {
			cmp_reg_reg(asm_gen->out, mfirst_data->reg_name,
				    msecond_data->reg_name);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == REGISTER) {
			mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
			cmp_reg_reg(asm_gen->out, temp_reg,
				    msecond_data->reg_name);
		}
		if (mfirst_data->location == REGISTER
		    && msecond_data->location == STACK) {
			cmp_reg_stack(asm_gen->out, mfirst_data->reg_name,
				      msecond_data->offset);
		}
		if (mfirst_data->location == STACK
		    && msecond_data->location == STACK) {
			mov_reg_stack(asm_gen->out, temp_reg, mfirst_data->offset);
			cmp_reg_stack(asm_gen->out, temp_reg,
				      msecond_data->offset);
		}
	} else {
		// first is variable, second is literal
		if (contains_key(table->variables, mfirst)) {
			VAR_DATA *mfirst_data =
			    hashmap_get(table->variables, mfirst);
			mov_reg_val(asm_gen->out, temp_reg, msecond);
			if (mfirst_data->location == REGISTER) {
				cmp_reg_reg(asm_gen->out, mfirst_data->reg_name,
					    temp_reg);
			}
			if (mfirst_data->location == STACK) {
				char *another_reg = ag_get_temp_reg(asm_gen);
				mov_reg_stack(asm_gen->out, another_reg, mfirst_data->offset);
				cmp_reg_reg(asm_gen->out, another_reg,
					    temp_reg);
			}
			// first is literal, second is variable
		} else if (contains_key(table->variables, msecond)) {
			VAR_DATA *msecond_data =
			    hashmap_get(table->variables, msecond);
			mov_reg_val(asm_gen->out, temp_reg, mfirst);
			if (msecond_data->location == REGISTER) {
				cmp_reg_reg(asm_gen->out, temp_reg,
					    msecond_data->reg_name);
			}
			if (msecond_data->location == STACK) {
				cmp_reg_stack(asm_gen->out, temp_reg,
					      msecond_data->offset);
			}
		} else {
			// TODO: What if they're both literals?
			mov_reg_val(asm_gen->out, temp_reg, mfirst);
			cmp_reg_val(asm_gen->out, temp_reg, msecond);
		}
	}
	ag_output_comp_result(asm_gen, quad->op, temp_reg);

	if (result_data->location == STACK) {
		mov_stack_reg(asm_gen->out, result_data->offset, temp_reg);
	}
	// TODO: deallocate register variables that can be deallocated
	ag_try_free_variable(asm_gen, mfirst, index);
	ag_try_free_variable(asm_gen, msecond, index);
}

void ag_output_comp_result(ASM_GENERATOR *asm_gen, char *op, char *temp_reg)
{
	int index = ag_get_temp_byte_reg_index(asm_gen);
	char *another_reg = regnames[index];
	char *another_reg_byte = regnames_byte[index];
	clear_reg(asm_gen->out, another_reg);
	if (strcmp(op, "=") == 0) {
		eq_flag_reg(asm_gen->out, temp_reg, another_reg_byte);
	} else if (strcmp(op, "<") == 0) {
		lt_flag_reg(asm_gen->out, temp_reg, another_reg_byte);
	} else if (strcmp(op, ">") == 0) {
		gt_flag_reg(asm_gen->out, temp_reg, another_reg_byte);
	} else if (strcmp(op, "<=") == 0) {
		lte_flag_reg(asm_gen->out, temp_reg, another_reg_byte);
	} else if (strcmp(op, ">=") == 0) {
		gte_flag_reg(asm_gen->out, temp_reg, another_reg_byte);
	}
	mov_reg_reg(asm_gen->out, temp_reg, another_reg);
}

void ag_preserve_registers(ASM_GENERATOR *asm_gen)
{
	for (int i = 0; i < REGCOUNT; i++) {
		if (hashmap_get(asm_gen->registers, regnames[i]) != NULL) {
			push_reg(asm_gen->out, regnames[i]);
		}
	}
}

void ag_restore_registers(ASM_GENERATOR *asm_gen)
{
	for (int i = REGCOUNT - 1; i >= 0; i--) {
		if (hashmap_get(asm_gen->registers, regnames[i]) != NULL) {
			pop_reg(asm_gen->out, regnames[i]);
		}
	}
}

char *ag_alloc_reg(ASM_GENERATOR *asm_gen, char *var)
{
	SYM_TABLE *table = asm_gen->sym_table;
	VAR_DATA *data = hashmap_get(table->variables, var);
	if (data->location != REGISTER || data->reg_name) {
		return NULL;
	}
	for (int i = 0; i < REGCOUNT; i++) {
		if (hashmap_get(asm_gen->registers, regnames[i]) == NULL) {
			hashmap_put(asm_gen->registers, regnames[i], var);
			data->reg_name = regnames[i];
			return regnames[i];
		}
	}
	// TODO: Allocate on stack if this returns NULL
	return NULL;
}

char *ag_get_temp_reg(ASM_GENERATOR *asm_gen)
{
	for (int i = 0; i < REGCOUNT; i++) {
		if (hashmap_get(asm_gen->registers, regnames[i]) == NULL) {
			return regnames[i];
		}
	}
	// TODO: Move variable to stack if no free register
	return NULL;
}

int ag_get_temp_byte_reg_index(ASM_GENERATOR *asm_gen)
{
	for (int i = 0; i < REGCOUNT; i++) {
		if (hashmap_get(asm_gen->registers, regnames[i]) == NULL) {
			if (regnames_byte[i] == NULL) {
				continue;
			}
			return i;
		}
	}
	// TODO: Move variable to stack if no free register
	return -1;
}

char *ag_realloc_reg(ASM_GENERATOR *asm_gen, char *reg)
{
	char *var_name = hashmap_get(asm_gen->registers, reg);
	if (var_name == NULL) {
		return reg;
	}
	char *reg_name = ag_alloc_reg(asm_gen, var_name);
	ag_free_reg(asm_gen, reg);
	return reg_name;
}

void ag_try_free_variable(ASM_GENERATOR *asm_gen, char *var, int index)
{
	SYM_TABLE *table = asm_gen->sym_table;
	if (contains_key(table->variables, var)) {
		VAR_DATA *var_data = hashmap_get(table->variables, var);
		if (var_data->location == REGISTER
		    && var_data->last_index == index) {
			ag_free_reg(asm_gen, var_data->reg_name);
		}
	}
}

void ag_free_reg(ASM_GENERATOR *asm_gen, char *reg)
{
	SYM_TABLE *table = asm_gen->sym_table;
	char *var_name = hashmap_get(asm_gen->registers, reg);
	hashmap_put(asm_gen->registers, reg, NULL);
	VAR_DATA *data = hashmap_get(table->variables, var_name);
	data->reg_name = NULL;
}

void ag_generate_code(ASM_GENERATOR *asm_gen)
{
	ag_index_variables(asm_gen);
	LIST *quads = asm_gen->quads;
	for (int i = 0; i < quads->size; i++) {
		QUAD *quad = list_get(asm_gen->quads, i);
		ag_quad_to_asm(asm_gen, quad, i);
	}
	ag_add_exit(asm_gen);
}

void ag_add_exit(ASM_GENERATOR *asm_gen)
{
	sb_append(asm_gen->out, "\tleave\n");
	sb_append(asm_gen->out, "\tmov rdi, 0\n");
	sb_append(asm_gen->out, "\tmov rax, 60\n");
	sb_append(asm_gen->out, "\tsyscall\n");
}

char *ag_get_code(ASM_GENERATOR *gen)
{
	char *out = sb_build(gen->out);
	return out;
}

