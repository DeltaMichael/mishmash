#ifndef ASM_TEMPLATES_H
#define ASM_TEMPLATES_H
#include "string_builder.h"

void mov_reg_reg(STRING_BUILDER * out, char *in_reg, char *out_reg);
void mov_reg_stack(STRING_BUILDER * out, char *in_reg, int out_offset);
void mov_stack_reg(STRING_BUILDER * out, int in_offset, char *out_reg);
void mov_stack_stack(STRING_BUILDER * out, int in_offset, int out_offset,
		     char *temp_reg);
void mov_val_reg(STRING_BUILDER * out, char *val, char *out_reg);
void mov_val_stack(STRING_BUILDER * out, char *val, int out_offset);

void mul_reg_reg(STRING_BUILDER * out, char *in_reg, char *out_reg);
void mul_reg_stack(STRING_BUILDER * out, char *in_reg, int out_offset,
		   char *temp_reg);
void mul_stack_reg(STRING_BUILDER * out, int in_offset, char *out_reg);
void mul_stack_stack(STRING_BUILDER * out, int in_offset, int out_offset,
		     char *temp_reg);
void mul_val_reg(STRING_BUILDER * out, char *val, char *out_reg);
void mul_val_stack(STRING_BUILDER * out, char *val, int out_offset,
		   char *temp_reg);

void add_reg_reg(STRING_BUILDER * out, char *in_reg, char *out_reg);
void add_reg_stack(STRING_BUILDER * out, char *in_reg, int out_offset,
		   char *temp_reg);
void add_stack_reg(STRING_BUILDER * out, int in_offset, char *out_reg);
void add_stack_stack(STRING_BUILDER * out, int in_offset, int out_offset,
		     char *temp_reg);
void add_val_reg(STRING_BUILDER * out, char *val, char *out_reg);
void add_val_stack(STRING_BUILDER * out, char *val, int out_offset,
		   char *temp_reg);

void neg_reg(STRING_BUILDER * out, char *reg);
void neg_stack(STRING_BUILDER * out, int offset, char *temp_reg);

void pop_reg(STRING_BUILDER * out, char *reg_name);
void push_reg(STRING_BUILDER * out, char *reg_name);
void call(STRING_BUILDER * out, char *function);

void cmp_reg_reg(STRING_BUILDER *out, char* first_reg, char* second_reg);
void cmp_stack_reg(STRING_BUILDER *out, int offset, char* reg);
void cmp_val_reg(STRING_BUILDER *out, char *val, char *reg);
void eq_flag_reg(STRING_BUILDER* out, char *reg, char* byte_reg);
void lt_flag_reg(STRING_BUILDER *out, char *reg, char* byte_reg);
void clear_reg(STRING_BUILDER *out, char *reg);

#endif

