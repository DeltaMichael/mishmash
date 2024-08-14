#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

AST_EXPR* ast_expr_init(AST_EXPR_TYPE type, TOKEN_TYPE op_type, char* op) {
	AST_EXPR* expr = malloc(sizeof(AST_EXPR));
	expr->type = type;
	expr->children = init_list(sizeof(AST_EXPR));
	expr->op_type = op_type;
	expr->op = op;
	expr->is_terminated = false;
	return expr;
}

void ast_expr_print(AST_EXPR* expr) {
	printf("[");
	printf("%s", expr->op);
	if (expr->children != NULL) {
		for (int i = 0; i < expr->children->size; i++) {
			ast_expr_print(list_get(expr->children, i));
		}
	}
	printf("]");
}

