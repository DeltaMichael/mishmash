#include "include/tac_generator.h"
#include "include/list.h"
#include <stdio.h>


QUAD* init_quad(char* op, char* arg1, char* arg2, char* result) {
	QUAD* quad = malloc(sizeof(QUAD));
	quad->op = op;
	quad->arg1 = arg1;
	quad->arg2 = arg2;
	quad->result = result;
	return quad;
}

QUAD* quad_from_expr(AST_EXPR* expr, LIST* quads, SYM_TABLE* table) {
	if(expr->type == PRIMARY) {
		return init_quad(NULL, NULL, NULL, expr->op->lexeme);
	}
	if(expr->type == UNARY) {
		AST_EXPR* arg = list_get(expr->children, 0);
		QUAD* argquad = quad_from_expr(arg, quads, table);
		// TODO: Figure out a way to generate temp variable names
		char* res_name = get_temp(table);
		QUAD* uquad = init_quad(expr->op->lexeme, argquad->result, NULL, res_name);
		list_push(quads, uquad);
		return uquad;
	}
	if(expr->type == BINARY) {
		AST_EXPR* left = list_get(expr->children, 0);
		AST_EXPR* right = list_get(expr->children, 1);
		QUAD* left_quad = quad_from_expr(left, quads, table);
		QUAD* right_quad = quad_from_expr(right, quads, table);
		// TODO: Figure out a way to generate temp variable names
		char* res_name = get_temp(table);
		QUAD* bquad = init_quad(expr->op->lexeme, left_quad->result, right_quad->result, res_name);
		list_push(quads, bquad);
		return bquad;
	}
}

QUAD* quad_from_stmt(AST_STMT* stmt, LIST* quads, SYM_TABLE* table) {
	if(stmt->type == EXPRESSION_STATEMENT) {
		return quad_from_expr(list_get(stmt->values, 0), quads, table);
	}
	if(stmt->type == ASSIGNMENT) {
		QUAD* value = quad_from_stmt(list_get(stmt->values, 0), quads, table);
		QUAD* aquad = init_quad(":=", value->result, NULL, stmt->id->lexeme);
		list_push(quads, aquad);
		return aquad;
	}
	if(stmt->type == BLOCK) {
		LIST* values = stmt->values;
		for(int i = 0; i < values->size; i++) {
			quad_from_stmt(list_get(values, i), quads, table);
		}
	}
}


SYM_TABLE* init_sym_table(SYM_TABLE* enclosing) {
	SYM_TABLE* table = malloc(sizeof(SYM_TABLE));
	table->index = 0;
	if(enclosing == NULL) {
		table->depth = 0;
	} else {
		table->depth = enclosing->depth + 1;
	}
	table->enclosing = enclosing;
	return table;
}

char* get_temp(SYM_TABLE* table) {
	int index = 10 * table->depth + table->index;
	char* buf = malloc(5 * sizeof(char));
	snprintf(buf, 5, "t%d", index);
	table->index++;
	return buf;
}

