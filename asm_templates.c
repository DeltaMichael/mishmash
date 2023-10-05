#include "include/asm_templates.h"
#include "include/string_builder.h"

void mov_reg_reg(STRING_BUILDER* out, char* in_reg, char* out_reg) {
	sb_append(out, "\tmovq %");
	sb_append(out, in_reg);
	sb_append(out, ", %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mov_reg_stack(STRING_BUILDER* out, char* in_reg, int out_offset) {
	sb_append(out, "\tmovq %");
	sb_append(out, in_reg);
	sb_append(out, ", -");
	sb_append_int(out, out_offset);
	sb_append(out, "(%rbp)\n");
}

void mov_stack_reg(STRING_BUILDER* out, int in_offset, char* out_reg) {
	sb_append(out, "\tmovq -");
	sb_append_int(out, in_offset);
	sb_append(out, "(%rbp), %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mov_stack_stack(STRING_BUILDER* out, int in_offset, int out_offset, char* temp_reg) {
	mov_stack_reg(out, in_offset, temp_reg);
	mov_reg_stack(out, temp_reg, out_offset);
}

void mov_val_reg(STRING_BUILDER* out, char* val, char* out_reg) {
	sb_append(out, "\tmovq $");
	sb_append(out, val);
	sb_append(out, ", %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mov_val_stack(STRING_BUILDER* out, char* val, int out_offset) {
	sb_append(out, "\tmovq $");
	sb_append(out, val);
	sb_append(out, ", -");
	sb_append_int(out, out_offset);
	sb_append(out, "(%rbp)\n");
}

void mul_reg_reg(STRING_BUILDER* out, char* in_reg, char* out_reg) {
	sb_append(out, "\timul %");
	sb_append(out, in_reg);
	sb_append(out, ", %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mul_reg_stack(STRING_BUILDER* out, char* in_reg, int out_offset, char* temp_reg) {
	mov_stack_reg(out, out_offset, temp_reg);
	mul_reg_reg(out, in_reg, temp_reg);
}

void mul_stack_reg(STRING_BUILDER* out, int in_offset, char* out_reg) {
	sb_append(out, "\timul -");
	sb_append_int(out, in_offset);
	sb_append(out, "(%rbp), %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mul_stack_stack(STRING_BUILDER* out, int in_offset, int out_offset, char* temp_reg) {
	mov_stack_reg(out, in_offset, temp_reg);
	mul_stack_reg(out, out_offset, temp_reg);
}

void mul_val_reg(STRING_BUILDER* out, char* val, char* out_reg) {
	sb_append(out, "\timul $");
	sb_append(out, val);
	sb_append(out, ", %");
	sb_append(out, out_reg);
	sb_append(out, "\n");
}

void mul_val_stack(STRING_BUILDER* out, char* val, int out_offset, char* temp_reg) {
	mov_stack_reg(out, out_offset, temp_reg);
	mul_val_reg(out, val, temp_reg);
}

void neg_reg(STRING_BUILDER* out, char* reg) {
	sb_append(out, "\tneg %");
	sb_append(out, reg);
	sb_append(out, "\n");
}

void neg_stack(STRING_BUILDER* out, int offset, char* temp_reg) {
	mov_stack_reg(out, offset, temp_reg);
	neg_reg(out, temp_reg);
	mov_reg_stack(out, temp_reg, offset);
}

