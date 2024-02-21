#ifndef ASM_TEMPLATES_H
#define ASM_TEMPLATES_H
#include "string_builder.h"

void mov_reg_reg(STRING_BUILDER * out, char *out_reg, char *in_reg);
void mov_stack_reg(STRING_BUILDER * out, int out_offset, char *in_reg);
void mov_reg_stack(STRING_BUILDER * out, char *out_reg, int in_offset);
void mov_stack_stack(STRING_BUILDER * out, int out_offset, int in_offset,
		     char *temp_reg);
void mov_reg_val(STRING_BUILDER *out, char *out_reg, char *val);
void mov_stack_val(STRING_BUILDER *out, int out_offset, char *val);

void mul_reg_reg(STRING_BUILDER *out, char *out_reg, char *in_reg);
void mul_stack_reg(STRING_BUILDER *out, int out_offset, char *in_reg, char *temp_reg);
void mul_reg_stack(STRING_BUILDER *out, char* out_reg, int in_offset);
void mul_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset, char *temp_reg);
void mul_reg_val(STRING_BUILDER *out, char *out_reg, char *val);
void mul_stack_val(STRING_BUILDER *out, int out_offset, char *val, char *temp_reg);

void add_reg_reg(STRING_BUILDER *out, char *out_reg, char *in_reg);
void add_stack_reg(STRING_BUILDER *out, int out_offset, char *in_reg, char *temp_reg);
void add_reg_stack(STRING_BUILDER *out, char* out_reg, int in_offset);
void add_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset, char *temp_reg);
void add_reg_val(STRING_BUILDER *out, char *out_reg, char *val);
void add_stack_val(STRING_BUILDER *out, int out_offset, char *val, char *temp_reg);

void neg_reg(STRING_BUILDER * out, char *reg);
void neg_stack(STRING_BUILDER * out, int offset, char *temp_reg);

void pop_reg(STRING_BUILDER * out, char *reg_name);
void push_reg(STRING_BUILDER * out, char *reg_name);
void call(STRING_BUILDER * out, char *function);

void cmp_reg_reg(STRING_BUILDER * out, char *first_reg, char *second_reg);
void cmp_reg_stack(STRING_BUILDER * out, char *reg, int offset);
void cmp_reg_val(STRING_BUILDER * out, char *reg, char *val);

void sub_reg_reg(STRING_BUILDER * out, char *first_reg, char *second_reg);
void sub_reg_stack(STRING_BUILDER * out, char *reg, int offset);
void sub_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset, char *temp_reg);
void sub_reg_val(STRING_BUILDER * out, char *reg, char *val);

void div_reg_reg(STRING_BUILDER * out, char *first_reg, char *second_reg);
void div_reg_stack(STRING_BUILDER * out, char *reg, int offset);
void div_stack_stack(STRING_BUILDER *out, int out_offset, int in_offset, char *temp_reg);
void div_reg_val(STRING_BUILDER * out, char *reg, char *val);

void eq_flag_reg(STRING_BUILDER *out, char *result_reg, char *true_reg);
void lt_flag_reg(STRING_BUILDER *out, char *result_reg, char *true_reg);
void lte_flag_reg(STRING_BUILDER *out, char *result_reg, char *true_reg);
void gt_flag_reg(STRING_BUILDER *out, char *result_reg, char *true_reg);
void gte_flag_reg(STRING_BUILDER *out, char *result_reg, char *true_reg);
void clear_reg(STRING_BUILDER * out, char *reg);

void test_reg_goto_false(STRING_BUILDER *out, char *reg, char *label);
void test_stack_goto_false(STRING_BUILDER *out, int offset, char *temp_reg, char *label);
void goto_label(STRING_BUILDER *out, char *label);
void insert_label(STRING_BUILDER *out, char *label);

#endif

