#ifndef AST_H
#define AST_H

#include "../common/list.h"
#include "generated/token.h"

typedef enum {
	BLOCK,
	LINE,
	ASSIGNMENT,
	EXPRESSION,
	COMPARISON,
	FACTOR,
	TERM,
	UNARY,
	VAR_DECLR,
	FUNC_CALL,
	BASIC
} AST_EXPR_TYPE;

typedef struct {
	AST_EXPR_TYPE type;
	char* op;
	TOKEN_TYPE op_type;
	LIST* children;
	bool is_terminated;
} AST_EXPR;

AST_EXPR* ast_expr_init(AST_EXPR_TYPE type, TOKEN_TYPE op_type, char* op);
void ast_expr_print(AST_EXPR* expr);

#endif

