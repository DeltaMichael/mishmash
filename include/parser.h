#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "list.h"
#include <ctype.h>
#include <stdbool.h>

typedef struct {
	TOKEN* current;
	LIST* tokens;
	size_t size;
} PARSER;

typedef enum {
	BINARY,
	UNARY,
	PRIMARY
} AST_EXPR_TYPE;

typedef struct {
	TOKEN* op;
	AST_EXPR_TYPE type;
	LIST* children;
} AST_EXPR;

PARSER* init_parser(LIST* tokens);
AST_EXPR* init_ast_expr(TOKEN* op, AST_EXPR_TYPE type, LIST* children);
TOKEN* parser_peek(PARSER* parser);
void parser_eat(PARSER* parser, TOKEN_TYPE type);
bool parser_match(PARSER* parser, TOKEN_TYPE type);

#endif

