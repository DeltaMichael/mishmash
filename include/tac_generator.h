#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include "list.h"
#include "parser.h"

typedef struct SYM_TABLE {
	int index;
	int depth;
	struct SYM_TABLE* enclosing;
} SYM_TABLE;

typedef struct {
	char* op;
	char* arg1;
	char* arg2;
	char* result;
} QUAD;

QUAD* init_quad(char* op, char* arg1, char* arg2, char* result);
QUAD* quad_from_expr(AST_EXPR* expr, LIST* quads, SYM_TABLE* sym_table);
QUAD* quad_from_stmt(AST_STMT* stmt, LIST* quads, SYM_TABLE* sym_table);

SYM_TABLE* init_sym_table(SYM_TABLE* enclosing);
char* get_temp(SYM_TABLE* table);

#endif

