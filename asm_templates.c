#include "include/asm_templates.h"
#include "include/string_builder.h"
#include <stdio.h>
#include <string.h>

void mov_reg_reg(STRING_BUILDER *out, char *out_reg, char *in_reg)
{
	sb_append(out, "\tmov ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, in_reg);
	sb_append(out, "\n");
}

void mov_stack_reg(STRING_BUILDER *out, int out_offset, char* in_reg)
{
	sb_append(out, "\tmov qword [rbp - ");
	sb_append_int(out, out_offset);
	sb_append(out, "], ");
	sb_append(out, in_reg);
	sb_append(out, "\n");
}

void mov_reg_stack(STRING_BUILDER *out, char *out_reg, int in_offset)
{
	sb_append(out, "\tmov qword ");
	sb_append(out, out_reg);
	sb_append(out, ", [rbp - ");
	sb_append_int(out, in_offset);
	sb_append(out, "]\n");
}

void mov_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset,
		     char *temp_reg)
{
	mov_reg_stack(out, temp_reg, in_offset);
	mov_stack_reg(out, out_offset, temp_reg);
}

void mov_reg_val(STRING_BUILDER *out, char *out_reg, char* val)
{
	sb_append(out, "\tmov ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, val);
	sb_append(out, "\n");
}

void mov_stack_val(STRING_BUILDER *out, int out_offset, char* val)
{
	sb_append(out, "\tmov qword [rbp - ");
	sb_append_int(out, out_offset);
	sb_append(out, "], ");
	sb_append(out, val);
	sb_append(out, "\n");
}

void mul_reg_reg(STRING_BUILDER *out, char *out_reg, char *in_reg)
{
	sb_append(out, "\timul ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, in_reg);
	sb_append(out, "\n");
}

void mul_stack_reg(STRING_BUILDER *out, int out_offset, char* in_reg, char *temp_reg)
{
	mov_reg_stack(out, temp_reg, out_offset);
	mul_reg_reg(out, temp_reg, in_reg);
}

void mul_reg_stack(STRING_BUILDER *out, char *out_reg, int in_offset)
{
	sb_append(out, "\timul ");
	sb_append(out, out_reg);
	sb_append(out, ", [rbp - ");
	sb_append_int(out, in_offset);
	sb_append(out, "]\n");
}

void mul_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset,
		     char *temp_reg)
{
	mov_reg_stack(out, temp_reg, in_offset);
	mul_reg_stack(out, temp_reg, out_offset);
}

void mul_reg_val(STRING_BUILDER *out, char *out_reg, char *val)
{
	sb_append(out, "\timul ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, val);
	sb_append(out, "\n");
}

void mul_stack_val(STRING_BUILDER *out, int out_offset, char* val, char *temp_reg)
{
	mov_reg_stack(out, temp_reg, out_offset);
	mul_reg_val(out, temp_reg, val);
}

void add_reg_reg(STRING_BUILDER *out, char *out_reg, char *in_reg)
{
	sb_append(out, "\tadd ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, in_reg);
	sb_append(out, "\n");
}

void add_stack_reg(STRING_BUILDER *out, int out_offset, char *in_reg, char *temp_reg)
{
	mov_reg_stack(out, temp_reg, out_offset);
	add_reg_reg(out, temp_reg, in_reg);
}

void add_reg_stack(STRING_BUILDER *out, char *out_reg, int in_offset)
{
	sb_append(out, "\tadd ");
	sb_append(out, out_reg);
	sb_append(out, ", [rbp - ");
	sb_append_int(out, in_offset);
	sb_append(out, "]\n");
}

void add_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset,
		     char *temp_reg)
{
	mov_reg_stack(out, temp_reg, in_offset);
	add_reg_stack(out, temp_reg, out_offset);
}

void add_reg_val(STRING_BUILDER *out, char *out_reg, char *val)
{
	sb_append(out, "\tadd ");
	sb_append(out, out_reg);
	sb_append(out, ", ");
	sb_append(out, val);
	sb_append(out, "\n");
}

void add_stack_val(STRING_BUILDER *out, int out_offset, char *val,
		   char *temp_reg)
{
	mov_reg_stack(out, temp_reg, out_offset);
	add_reg_val(out, temp_reg, val);
}

void neg_reg(STRING_BUILDER *out, char *reg)
{
	sb_append(out, "\tneg ");
	sb_append(out, reg);
	sb_append(out, "\n");
}

void neg_stack(STRING_BUILDER *out, int offset, char *temp_reg)
{
	mov_reg_stack(out, temp_reg, offset);
	neg_reg(out, temp_reg);
	mov_stack_reg(out, offset, temp_reg);
}

void push_reg(STRING_BUILDER *out, char *reg_name)
{
	sb_append(out, "\tpush ");
	sb_append(out, reg_name);
	sb_append(out, "\n");
}

void pop_reg(STRING_BUILDER *out, char *reg_name)
{
	sb_append(out, "\tpop ");
	sb_append(out, reg_name);
	sb_append(out, "\n");
}

void call(STRING_BUILDER *out, char *function)
{
	sb_append(out, "\tcall ");
	sb_append(out, function);
	sb_append(out, "\n");
}

void cmp_reg_reg(STRING_BUILDER *out, char *first_reg, char *second_reg)
{
	sb_append(out, "\tcmp ");
	sb_append(out, first_reg);
	sb_append(out, ", ");
	sb_append(out, second_reg);
	sb_append(out, "\n");
}

void cmp_reg_stack(STRING_BUILDER *out, char *reg, int offset)
{
	sb_append(out, "\tcmp ");
	sb_append(out, reg);
	sb_append(out, ", [rbp - ");
	sb_append_int(out, offset);
	sb_append(out, "]\n");
}

void cmp_reg_val(STRING_BUILDER *out, char *reg, char *val)
{
	sb_append(out, "\tcmp ");
	sb_append(out, reg);
	sb_append(out, ", ");
	sb_append(out, val);
	sb_append(out, "\n");
}

void sub_reg_reg(STRING_BUILDER *out, char *first_reg, char *second_reg)
{
	sb_append(out, "\tsub ");
	sb_append(out, first_reg);
	sb_append(out, ", ");
	sb_append(out, second_reg);
	sb_append(out, "\n");
}

void sub_reg_stack(STRING_BUILDER *out, char *reg, int offset)
{
	sb_append(out, "\tsub ");
	sb_append(out, reg);
	sb_append(out, ", [rbp - ");
	sb_append_int(out, offset);
	sb_append(out, "]\n");
}

void sub_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset,
		     char *temp_reg)
{
	mov_reg_stack(out, temp_reg, in_offset);
	sub_reg_stack(out, temp_reg, out_offset);
}

void sub_reg_val(STRING_BUILDER *out, char *reg, char *val)
{
	sb_append(out, "\tsub ");
	sb_append(out, reg);
	sb_append(out, ", ");
	sb_append(out, val);
	sb_append(out, "\n");
}

// Make sure the contents of rax and rdx are saved before calling this
void div_reg_reg(STRING_BUILDER *out, char *first_reg, char *second_reg)
{
	if(strcmp("rax", first_reg) != 0) {
		mov_reg_reg(out, "rax", first_reg);
	}

	sb_append(out, "\tcqo\n");
	sb_append(out, "\tidiv ");
	sb_append(out, second_reg);
	sb_append(out, "\n");

	if(strcmp("rax", first_reg) != 0) {
		mov_reg_reg(out, first_reg, "rax");
	}
}

// Make sure the contents of rax and rdx are saved before calling this
void div_reg_stack(STRING_BUILDER *out, char *reg, int offset)
{
	if(strcmp("rax", reg) != 0) {
		mov_reg_reg(out, "rax", reg);
	}

	sb_append(out, "\tcqo\n");
	sb_append(out, "\tidiv qword [rbp - ");
	sb_append_int(out, offset);
	sb_append(out, "]\n");

	if(strcmp("rax", reg) != 0) {
		mov_reg_reg(out, reg, "rax");
	}

}

// Make sure the contents of rax and rdx are saved before calling this
void div_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset,
		     char *temp_reg)
{
	mov_reg_stack(out, temp_reg, in_offset);
	div_reg_stack(out, temp_reg, out_offset);
}

// Make sure the contents of rax and rdx are saved before calling this
void div_reg_val(STRING_BUILDER *out, char *reg, char *val)
{
	sb_append(out, "\tpush qword ");
	sb_append(out, val);
	sb_append(out, "\n");

	if(strcmp("rax", reg) != 0) {
		mov_reg_reg(out, "rax", reg);
	}

	sb_append(out, "\tcqo\n");
	sb_append(out, "\tidiv qword [rsp]\n");

	if(strcmp("rax", reg) != 0) {
		mov_reg_reg(out, reg, "rax");
	}

	sb_append(out, "\tadd rsp, 8\n");
}

void eq_flag_reg(STRING_BUILDER *out, char *byte_reg)
{
	sb_append(out, "\tsete ");
	sb_append(out, byte_reg);
	sb_append(out, "\n");
}

void lt_flag_reg(STRING_BUILDER *out, char *byte_reg)
{
	sb_append(out, "\tsetl ");
	sb_append(out, byte_reg);
	sb_append(out, "\n");
}

void lte_flag_reg(STRING_BUILDER *out, char *byte_reg)
{
	sb_append(out, "\tsetle ");
	sb_append(out, byte_reg);
	sb_append(out, "\n");
}

void gt_flag_reg(STRING_BUILDER *out, char *byte_reg)
{
	sb_append(out, "\tsetg ");
	sb_append(out, byte_reg);
	sb_append(out, "\n");
}

void gte_flag_reg(STRING_BUILDER *out, char *byte_reg)
{
	sb_append(out, "\tsetge ");
	sb_append(out, byte_reg);
	sb_append(out, "\n");
}

void clear_reg(STRING_BUILDER *out, char *reg)
{
	sb_append(out, "\tmov ");
	sb_append(out, reg);
	sb_append(out, ", 0\n");
}

