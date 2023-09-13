#include "include/ast_printer.h"
#include <stdio.h>
void print_ast_expr(AST_EXPR* root) {
	if(!root->children) {
		printf("%s ", root->op->lexeme);
		return;
	} else {
		printf("(");
		for(int i = 0; i < root->children->size; i++) {
			print_ast_expr(root->children->elements[i]);
		}
	}
	printf("%s) ", root->op->lexeme);
}

void print_ast_stmt(AST_STMT* root) {
	if (root->type == EXPRESSION_STATEMENT) {
		print_ast_expr(list_get(root->values, 0));
		printf("\n");
	}
	if (root->type == BLOCK) {
		printf("BLOCK START\n");
		LIST* values = root->values;
		for(int i = 0; i < values->size; i++) {
			print_ast_stmt(list_get(values, i));
		}
		printf("BLOCK END\n");
	}
	if (root->type == ASSIGNMENT) {
		printf("%s := ", root->id->lexeme);
		AST_STMT* assignee = list_get(root->values, 0);
		print_ast_stmt(assignee);
	}
	if (root->type == DECLARATION) {
		TOKEN* type = list_get(root->values, 0);
		printf("declare %s of type %s\n", root->id->lexeme, type->lexeme);
	}

}

