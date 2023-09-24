#include "include/asm_generator.h"
#include "include/hashmap.h"
#include "include/string_builder.h"

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
	return gen;
}

char* ag_get_code(ASM_GENERATOR* gen) {
	char* out = sb_build(gen->out);
	return out;
}

