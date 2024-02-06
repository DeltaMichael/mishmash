#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "include/parser.h"
#include "include/list.h"
#include "include/ast_printer.h"

PARSER *init_parser(LIST *tokens)
{
	PARSER *parser = malloc(sizeof(PARSER));
	parser->tokens = tokens;
	parser->index = 0;
	parser->current = (TOKEN *) tokens->elements[0];
	parser->size = tokens->size;
	parser->has_error = 0;
	parser->exited_with_error = 0;
	return parser;
}

void free_parser(PARSER *parser)
{
	free(parser);
	parser = NULL;
}

AST_EXPR *init_ast_expr(TOKEN *op, AST_EXPR_TYPE type, LIST *children)
{
	AST_EXPR *node = malloc(sizeof(AST_EXPR));
	node->op = op;
	node->children = children;
	node->type = type;
	return node;
}

AST_STMT *init_ast_stmt(AST_STMT_TYPE type, LIST *values, TOKEN *id)
{
	AST_STMT *node = malloc(sizeof(AST_STMT));
	node->id = id;
	node->values = values;
	node->type = type;
	return node;
}

void free_ast_expr(AST_EXPR *root)
{
	if (root == NULL) {
		return;
	}
	if (!root->children) {
		free(root);
		root = NULL;
		return;
	} else {
		for (int i = 0; i < root->children->size; i++) {
			free_ast_expr(root->children->elements[i]);
		}
	}
	free(root->children->elements);
	root->children->elements = NULL;
	free(root->children);
	root->children = NULL;
	free(root);
	root = NULL;
}

void free_ast_stmt(AST_STMT *root)
{
	if (root == NULL) {
		return;
	}
	if (root->type == EXPRESSION_STATEMENT) {
		for (int i = 0; i < root->values->size; i++) {
			free_ast_expr(root->values->elements[i]);
		}
	}
	if (root->type != DECLARATION && root->type != EXPRESSION_STATEMENT) {
		if (root->values) {
			for (int i = 0; i < root->values->size; i++) {
				free_ast_stmt(root->values->elements[i]);
			}
		}
	}
	if (root->values && root->values->elements) {
		free(root->values->elements);
		root->values->elements = NULL;
	}
	if (root->values) {
		free(root->values);
		root->values = NULL;
	}
	free(root);
	root = NULL;
}

void parser_advance(PARSER *parser)
{
	parser->index++;
	if (parser->index < parser->size) {
		parser->current = parser->tokens->elements[parser->index];
	}
}

TOKEN *parser_peek(PARSER *parser)
{
	if (parser->index < parser->size - 1) {
		return parser->tokens->elements[parser->index + 1];
	}
	return parser->current;
}

TOKEN *parser_previous(PARSER *parser)
{
	if (parser->index > 0) {
		return parser->tokens->elements[parser->index - 1];
	}
	return parser->current;
}

void parser_error_at(PARSER *parser, TOKEN* token, char *message)
{
	fprintf(stderr, "error: line %d: near: '%s': ", token->line + 1,
		token->lexeme);
	fprintf(stderr, "%s\n", message);
	parser->has_error = 1;
	parser->exited_with_error = 1;
}

void parser_error(PARSER *parser, char *message)
{
	TOKEN *prev = parser_previous(parser);
	parser_error_at(parser, prev, message);
}

bool parser_match(PARSER *parser, TOKEN_TYPE type)
{
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

void parser_eat(PARSER *parser, TOKEN_TYPE type, char *message)
{
	if (!parser_match(parser, type)) {
		parser_error(parser, message);
	}
}

bool parser_is_at_end(PARSER *parser)
{
	return parser->index >= parser->size;
}

void parser_sync(PARSER *parser)
{
	parser_advance(parser);
	while (!parser_is_at_end(parser)) {
		if (parser_previous(parser)->type == LINE_TERM) {
			return;
		}
		switch (parser_peek(parser)->type) {
		case FUN:
		case BEGIN:
		case END:
		case DECLR:
		case RETRUN:
			return;
		}
		parser_advance(parser);
	}
}

AST_STMT *parser_parse(PARSER *parser)
{
	LIST* statements = init_list(sizeof(AST_STMT*));
	while (!parser_is_at_end(parser)) {
		list_push(statements, statement(parser));
		if (parser->has_error) {
			parser->has_error = 0;
			parser_sync(parser);
		}
	}
	// If we have statements outside of the main block
	if (statements->size > 1) {
		for(int i = 0; i < statements->size; i++) {
			AST_STMT* stmt = list_get(statements, i);
			if(stmt->type != BLOCK) {
				parser_error_at(parser, stmt->id, "Unexpected statement outside of block");
			}
		}
	}
	// If there was an error, free everything
	// and return NULL
	if (parser->exited_with_error) {
		free_list(statements, free_ast_stmt);
		return NULL;
	}
	AST_STMT* out = list_get(statements, 0);
	return out;
}

AST_STMT *statement(PARSER *parser)
{
	return block(parser);
}

AST_STMT *block(PARSER *parser)
{
	if (parser_match(parser, DECLR)) {
		// parse declarations and block
		LIST *values = init_list(sizeof(AST_STMT *));
		while (parser->current->type != BEGIN
		       && !parser_is_at_end(parser)) {
			AST_STMT *stmt = statement(parser);
			if (stmt->type != DECLARATION) {
				parser_error(parser,
					     "Only declarations allowed in block DECLR header");
			}
			list_push(values, stmt);
		}
		parser_eat(parser, BEGIN, "Expected BEGIN after DECLR");
		return block_body(parser, values);
	}
	if (parser_match(parser, BEGIN)) {
		// just parse block
		LIST *values = init_list(sizeof(AST_STMT *));
		return block_body(parser, values);
	}
	return print_stmt(parser);
}

AST_STMT *block_body(PARSER *parser, LIST *values)
{
	while (parser->current->type != END && !parser_is_at_end(parser)) {
		AST_STMT *stmt = statement(parser);
		if (stmt->type == DECLARATION) {
			parser_error(parser,
				     "Declaration outside block DECLR header");
		}
		list_push(values, stmt);
	}
	AST_STMT *stmt = init_ast_stmt(BLOCK, values, NULL);
	parser_eat(parser, END, "Expected 'END' at the end of block");
	return stmt;
}

AST_STMT *print_stmt(PARSER *parser)
{
	if (parser_match(parser, PRINT_OP)) {
		AST_STMT *stmt = expression_statement(parser);
		stmt->type = PRINT_STATEMENT;
		return stmt;
	}
	return declaration(parser);
}

// TODO: Move this down the parse tree
AST_STMT *declaration(PARSER *parser)
{
	if (parser->current->type == IDENTIFIER
	    && parser_peek(parser)->type == COLON) {
		parser_match(parser, IDENTIFIER);
		TOKEN *id = parser_previous(parser);
		parser_match(parser, COLON);
		if (parser_match(parser, STATIC_TYPE)) {
			TOKEN *type = parser_previous(parser);
			LIST *values = init_list(sizeof(TOKEN *));
			list_push(values, type);
			AST_STMT *stmt = init_ast_stmt(DECLARATION, values, id);
			parser_eat(parser, LINE_TERM,
				   "Expected line terminator at end of line");
			return stmt;
		}
		parser_error(parser, "Invalid static type");
	}
	return assignment(parser);
}

AST_STMT *assignment(PARSER *parser)
{
	if (parser->current->type == IDENTIFIER
	    && parser_peek(parser)->type == ASSIGN) {
		parser_match(parser, IDENTIFIER);
		TOKEN *id = parser_previous(parser);
		parser_match(parser, ASSIGN);
		AST_STMT *assigned = statement(parser);
		// TODO: Add semantic checks for assignment, e.g. can't assign block to variable
		LIST *values = init_list(sizeof(AST_STMT *));
		list_push(values, assigned);
		AST_STMT *stmt = init_ast_stmt(ASSIGNMENT, values, id);
		return stmt;
	}
	return expression_statement(parser);
}

AST_STMT *expression_statement(PARSER *parser)
{
	LIST *values = init_list(sizeof(AST_EXPR *));
	list_push(values, expression(parser));
	AST_STMT *stmt = init_ast_stmt(EXPRESSION_STATEMENT, values, NULL);
	parser_eat(parser, LINE_TERM,
		   "Expected line terminator at end of line");
	return stmt;
}

AST_EXPR *expression(PARSER *parser)
{
	return equality(parser);
}

AST_EXPR *equality(PARSER *parser)
{
	AST_EXPR *left = term(parser);
	AST_EXPR *right;
	while (parser_match(parser, EQUALS) || parser_match(parser, LESS_THAN) || parser_match(parser, GREATER_THAN)) {
		TOKEN *op = parser_previous(parser);
		right = term(parser);
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;
}

// TODO: Add comparison

AST_EXPR *term(PARSER *parser)
{
	AST_EXPR *left = factor(parser);
	AST_EXPR *right;
	while (parser_match(parser, PLUS) || parser_match(parser, MINUS)) {
		TOKEN *op = parser_previous(parser);
		right = factor(parser);
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;

}

AST_EXPR *factor(PARSER *parser)
{
	AST_EXPR *left = unary(parser);
	AST_EXPR *right;
	while (parser_match(parser, MULT) || parser_match(parser, DIV)) {
		TOKEN *op = parser_previous(parser);
		right = unary(parser);
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;

}

AST_EXPR *unary(PARSER *parser)
{
	if (parser_match(parser, MINUS)) {
		TOKEN *op = parser_previous(parser);
		AST_EXPR *right = unary(parser);
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, right);
		return init_ast_expr(op, UNARY, children);
	}
	return primary(parser);
}

AST_EXPR *primary(PARSER *parser)
{
	if (parser_match(parser, INT_LITERAL)
	    || parser_match(parser, IDENTIFIER)) {
		return init_ast_expr(parser_previous(parser), PRIMARY, NULL);
	}
	if (parser_match(parser, LEFT_PAREN)) {
		AST_EXPR *expr = expression(parser);
		parser_eat(parser, RIGHT_PAREN, "Expected closing ')'");
		return expr;
	}
	parser_error(parser, "Unexpected token");
	return NULL;
}

