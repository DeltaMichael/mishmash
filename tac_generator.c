#include "include/tac_generator.h"
#include "include/list.h"
#include "include/hashmap.h"
#include <stdio.h>

QUAD *init_quad(char *op, char *arg1, char *arg2, char *result, bool uses_temp)
{
	QUAD *quad = malloc(sizeof(QUAD));
	quad->op = op;
	quad->arg1 = arg1;
	quad->arg2 = arg2;
	quad->result = result;
	// TODO: Figure out a better way to do this
	quad->uses_temp = uses_temp;
	return quad;
}

void free_quad(QUAD *quad)
{
	// if (quad->uses_temp) {
	//      free(quad->result);
	//      quad->result = NULL;
	// }
	free(quad);
}

QUAD *quad_from_expr(AST_EXPR *expr, LIST *quads, SYM_TABLE *table)
{
	if (expr->type == PRIMARY)
	{
		QUAD *pquad =
			init_quad("skip", NULL, NULL, expr->op->lexeme, false);
		list_push(quads, pquad);
		return pquad;
	}
	if (expr->type == UNARY)
	{
		AST_EXPR *arg = list_get(expr->children, 0);
		QUAD *argquad = quad_from_expr(arg, quads, table);
		char *res_name = symtable_get_temp(table);
		QUAD *uquad =
			init_quad("uminus", argquad->result, NULL, res_name, true);
		list_push(quads, uquad);
		return uquad;
	}
	if (expr->type == BINARY)
	{
		AST_EXPR *left = list_get(expr->children, 0);
		AST_EXPR *right = list_get(expr->children, 1);
		QUAD *left_quad = quad_from_expr(left, quads, table);
		QUAD *right_quad = quad_from_expr(right, quads, table);
		char *res_name = symtable_get_temp(table);
		QUAD *bquad = init_quad(expr->op->lexeme, left_quad->result,
								right_quad->result, res_name, true);
		list_push(quads, bquad);
		return bquad;
	}
	return NULL;
}

QUAD *quad_from_stmt(AST_STMT *stmt, LIST *quads, SYM_TABLE *table)
{
	if (stmt->type == PRINT_STATEMENT)
	{
		QUAD *expr_quad =
			quad_from_expr(list_get(stmt->values, 0), quads, table);
		QUAD *pquad =
			init_quad("print", expr_quad->result, NULL, NULL, false);
		list_push(quads, pquad);
		return pquad;
	}
	if (stmt->type == EXPRESSION_STATEMENT)
	{
		return quad_from_expr(list_get(stmt->values, 0), quads, table);
	}
	if (stmt->type == DECLARATION)
	{
		TOKEN *type = list_get(stmt->values, 0);
		symtable_init_var(table, stmt->id->lexeme, STACK, type->lexeme);
		return NULL;
	}
	if (stmt->type == ASSIGNMENT)
	{
		QUAD *value =
			quad_from_stmt(list_get(stmt->values, 0), quads, table);
		QUAD *aquad =
			init_quad(":=", value->result, NULL, stmt->id->lexeme,
					  false);
		list_push(quads, aquad);
		return aquad;
	}
	if (stmt->type == BLOCK)
	{
		LIST *values = stmt->values;
		for (int i = 0; i < values->size; i++)
		{
			quad_from_stmt(list_get(values, i), quads, table);
		}
		return NULL;
	}
	if (stmt->type == CONDITION)
	{
		// iffalse conditional goto false_label
		AST_STMT *condition = list_get(stmt->values, 0);
		QUAD *cquad = quad_from_expr(list_get(condition->values, 0), quads, table);
		QUAD *if_quad = init_quad("iffalse", cquad->result, "goto", NULL, false);
		list_push(quads, if_quad);

		AST_STMT *true_block = list_get(stmt->values, 1);
		quad_from_stmt(true_block, quads, table);
		QUAD *goto_end_quad = NULL;
		if (stmt->values->size == 3)
		{
			goto_end_quad = init_quad("goto", NULL, NULL, NULL, false);
			list_push(quads, goto_end_quad);
			// create false label quad
			char *false_label = get_label('F', quads->size);
			QUAD *fl_quad = init_quad("label", false_label, NULL, NULL, false);
			list_push(quads, fl_quad);

			// backpatch false label
			if_quad->result = false_label;
			AST_STMT *false_block = list_get(stmt->values, 2);
			quad_from_stmt(false_block, quads, table);
		}

		// create end label quad
		char *end_label = get_label('E', quads->size);
		QUAD *el_quad = init_quad("label", end_label, NULL, NULL, false);
		list_push(quads, el_quad);

		// backpatch end label
		if (goto_end_quad)
		{
			goto_end_quad->arg1 = end_label;
		}
		if (stmt->values->size == 2)
		{
			if_quad->result = end_label;
		}
		return NULL;
	}
	if (stmt->type == LOOP)
	{
		// create start label
		char *start_label = get_label('S', quads->size);
		QUAD *s_quad = init_quad("label", start_label, NULL, NULL, false);
		list_push(quads, s_quad);

		// iffalse conditional goto loop exit label
		AST_STMT *condition = list_get(stmt->values, 0);
		QUAD *cquad = quad_from_expr(list_get(condition->values, 0), quads, table);
		QUAD *check_quad = init_quad("iffalse", cquad->result, "goto", NULL, false);
		list_push(quads, check_quad);

		AST_STMT *body = list_get(stmt->values, 1);
		quad_from_stmt(body, quads, table);

		QUAD *goto_start_quad = init_quad("goto", start_label, NULL, NULL, false);
		list_push(quads, goto_start_quad);

		// create end label quad
		char *end_label = get_label('E', quads->size);
		QUAD *el_quad = init_quad("label", end_label, NULL, NULL, false);
		list_push(quads, el_quad);

		// backpatch end label
		check_quad->result = end_label;
		return NULL;
	}

	return NULL;
}

VAR_DATA *init_var_data(VAR_LOCATION location, char *type, int offset)
{
	VAR_DATA *data = malloc(sizeof(VAR_DATA));
	data->location = location;
	data->type = type;
	data->reg_name = NULL;
	data->offset = offset;
	data->first_index = -1;
	data->last_index = -1;
	return data;
}

SYM_TABLE *init_symtable(SYM_TABLE *enclosing)
{
	SYM_TABLE *table = malloc(sizeof(SYM_TABLE));
	table->index = 0;
	if (enclosing == NULL)
	{
		table->depth = 0;
		table->offset = 0;
	}
	else
	{
		table->depth = enclosing->depth + 1;
		table->offset = enclosing->offset;
	}
	table->variables = init_hashmap();
	table->enclosing = enclosing;
	return table;
}

// TODO: This should also take a function pointer as argument
// For freeing the values
void free_symtable(SYM_TABLE *table)
{
	SYM_TABLE *current = table;
	while (current != NULL)
	{
		SYM_TABLE *next = current->enclosing;
		current->enclosing = NULL;
		free_hashmap(current->variables);
		current->variables = NULL;
		free(current);
		current = next;
	}
}

void symtable_init_var(SYM_TABLE *table, char *name, VAR_LOCATION location,
					   char *type)
{
	int offset = 0;
	if (location == STACK)
	{
		// TODO: add offset based on type
		offset = 8; // 8 bytes
	}
	table->offset += offset;
	VAR_DATA *data = init_var_data(location, type, table->offset);
	hashmap_put(table->variables, name, data);
}

char *symtable_get_temp(SYM_TABLE *table)
{
	int index = 100 * (table->depth + 1) + table->index;
	char *buf = malloc(5 * sizeof(char));
	snprintf(buf, 5, "t%d", index);
	symtable_init_var(table, buf, REGISTER, "int");
	table->index++;
	return buf;
}

char *get_label(char type, size_t index)
{
	int label_size = snprintf(NULL, 0, "%c%zu", type, index);
	char *buf = malloc((label_size + 1) * sizeof(char));
	sprintf(buf, "%c%zu", type, index);
	return buf;
}

