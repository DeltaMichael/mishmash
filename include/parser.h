#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "list.h"
#include <ctype.h>
#include <stdbool.h>

typedef struct {
	TOKEN* current;
	LIST* tokens;
	bool has_error;
	LIST* errors;
	size_t size;
	int index;
} PARSER;

typedef enum {
	BINARY,
	UNARY,
	PRIMARY
} AST_EXPR_TYPE;

// TODO: Add statements

typedef struct {
	TOKEN* op;
	AST_EXPR_TYPE type;
	LIST* children;
} AST_EXPR;

typedef enum {
	ASSIGNMENT,
	BLOCK,
	PRINT,
	EXPRESSION_STATEMENT
} AST_STMT_TYPE;

typedef struct {
	AST_STMT_TYPE type;
	LIST* values;
} AST_STMT;

PARSER* init_parser(LIST* tokens);
AST_EXPR* init_ast_expr(TOKEN* op, AST_EXPR_TYPE type, LIST* children);
TOKEN* parser_peek(PARSER* parser);
TOKEN* parser_previous(PARSER* parser);
void parser_eat(PARSER* parser, TOKEN_TYPE type, char* message);
bool parser_match(PARSER* parser, TOKEN_TYPE type);
bool parser_is_at_end(PARSER* parser);
void parser_advance(PARSER* parser);

AST_STMT* statement(PARSER* parser);
AST_STMT* expression_statement(PARSER* parser);
AST_EXPR* expression(PARSER* parser);
AST_EXPR* equality(PARSER* parser);
AST_EXPR* term(PARSER* parser);
AST_EXPR* factor(PARSER* parser);
AST_EXPR* unary(PARSER* parser);
AST_EXPR* primary(PARSER* parser);

#endif

