#include "include/ast_printer.h"
#include <stdio.h>
void print_ast_expr(AST_EXPR *root)
{
	if (!root->children)
	{
		printf("%s ", root->op->lexeme);
		return;
	}
	else
	{
		printf("[");
		for (int i = 0; i < root->children->size; i++)
		{
			print_ast_expr(root->children->elements[i]);
		}
	}
	printf("%s] ", root->op->lexeme);
}

void print_ast_stmt(AST_STMT *root, int depth)
{
	if (root->type == EXPRESSION_STATEMENT)
	{
		print_ast_expr(list_get(root->values, 0));
		printf("\n");
	}
	if (root->type == BLOCK)
	{
		printf("BLOCK START %d\n", depth);
		LIST *values = root->values;
		for (int i = 0; i < values->size; i++)
		{
			print_ast_stmt(list_get(values, i), depth + 1);
		}
		printf("BLOCK END %d\n", depth);
	}
	if (root->type == ASSIGNMENT)
	{
		printf("%s := ", root->id->lexeme);
		AST_STMT *assignee = list_get(root->values, 0);
		print_ast_stmt(assignee, depth);
	}
	if (root->type == DECLARATION)
	{
		AST_EXPR *type = list_get(root->values, 0);
		printf("declare %s of type ", root->id->lexeme);
		print_ast_expr(type);
		printf("\n");
	}
	if (root->type == PRINT_STATEMENT)
	{
		printf("print ");
		print_ast_expr(list_get(root->values, 0));
		printf("\n");
	}
	if (root->type == CONDITION)
	{
		TOKEN *type = list_get(root->values, 0);
		printf("\nif ");
		print_ast_stmt(list_get(root->values, 0), depth);
		print_ast_stmt(list_get(root->values, 1), depth);
		if (root->values->size == 3)
		{
			printf("\nelse ");
			print_ast_stmt(list_get(root->values, 2), depth);
		}
		printf("\n");
	}
	if (root->type == LOOP)
	{
		TOKEN *type = list_get(root->values, 0);
		printf("\nwhile ");
		print_ast_stmt(list_get(root->values, 0), depth);
		print_ast_stmt(list_get(root->values, 1), depth);
		printf("\n");
	}
}

