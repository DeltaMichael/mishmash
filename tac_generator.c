#include "include/tac_generator.h"
#include "include/list.h"
#include "include/hashmap.h"
#include <stdio.h>


QUAD* init_quad(char* op, char* arg1, char* arg2, char* result) {
	QUAD* quad = malloc(sizeof(QUAD));
	quad->op = op;
	quad->arg1 = arg1;
	quad->arg2 = arg2;
	quad->result = result;
	return quad;
}

void free_quad(QUAD* quad) {
	// if (quad->op != NULL) {
	// 	free(quad->op);
	// }
	// if (quad->arg1 != NULL) {
	// 	free(quad->arg1);
	// }
	// if (quad->arg2 != NULL) {
	// 	free(quad->arg2);
	// }
	// if (quad->result != NULL) {
	// 	free(quad->result);
	// }
	free(quad);
}

QUAD* quad_from_expr(AST_EXPR* expr, LIST* quads, SYM_TABLE* table) {
	if(expr->type == PRIMARY) {
		QUAD* pquad = init_quad("skip", NULL, NULL, expr->op->lexeme);
		list_push(quads, pquad);
		return pquad;
	}
	if(expr->type == UNARY) {
		AST_EXPR* arg = list_get(expr->children, 0);
		QUAD* argquad = quad_from_expr(arg, quads, table);
		char* res_name = symtable_get_temp(table);
       	QUAD* uquad = init_quad("uminus", argquad->result, NULL, res_name);
		list_push(quads, uquad);
		return uquad;
	}
	if(expr->type == BINARY) {
		AST_EXPR* left = list_get(expr->children, 0);
		AST_EXPR* right = list_get(expr->children, 1);
		QUAD* left_quad = quad_from_expr(left, quads, table);
		QUAD* right_quad = quad_from_expr(right, quads, table);
		char* res_name = symtable_get_temp(table);
		QUAD* bquad = init_quad(expr->op->lexeme, left_quad->result, right_quad->result, res_name);
		list_push(quads, bquad);
		return bquad;
	}
}

QUAD* quad_from_stmt(AST_STMT* stmt, LIST* quads, SYM_TABLE* table) {
	if(stmt->type == PRINT_STATEMENT) {
		QUAD* expr_quad = quad_from_expr(list_get(stmt->values, 0), quads, table);
		QUAD* pquad = init_quad("print", expr_quad->result, NULL, NULL);
		list_push(quads, pquad);
		return pquad;
	}
	if(stmt->type == EXPRESSION_STATEMENT) {
		return quad_from_expr(list_get(stmt->values, 0), quads, table);
	}
	if(stmt->type == DECLARATION) {
		TOKEN* type = list_get(stmt->values, 0);
		symtable_init_var(table, stmt->id->lexeme, STACK, type->lexeme);
		return NULL;
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

VAR_DATA* init_var_data(VAR_LOCATION location, char* type, int offset) {
	VAR_DATA* data = malloc(sizeof(VAR_DATA));
	data->location = location;
	data->type = type;
	data->reg_name = NULL;
	data->offset = offset;
	data->first_index = -1;
	data->last_index = -1;
}

SYM_TABLE* init_symtable(SYM_TABLE* enclosing) {
	SYM_TABLE* table = malloc(sizeof(SYM_TABLE));
	table->index = 0;
	if(enclosing == NULL) {
		table->depth = 0;
		table->offset = 0;
	} else {
		table->depth = enclosing->depth + 1;
		table->offset = enclosing->offset;
	}
	table->variables = init_hashmap();
	table->enclosing = enclosing;
	return table;
}

void symtable_init_var(SYM_TABLE* table, char* name, VAR_LOCATION location, char* type) {
	int offset = 0;
	if(location == STACK) {
		// TODO: add offset based on type
		offset = 64;
	}
	table->offset += offset;
	VAR_DATA* data = init_var_data(location, type, table->offset);
	hashmap_put(table->variables, name, data);
}

char* symtable_get_temp(SYM_TABLE* table) {
	int index = 100 * (table->depth + 1) + table->index;
	char* buf = malloc(5 * sizeof(char));
	snprintf(buf, 5, "t%d", index);
	symtable_init_var(table, buf, REGISTER, "int");
	table->index++;
	return buf;
}

