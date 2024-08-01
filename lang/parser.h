#ifndef PARSER_H
#define PARSER_H

#include "../common/list.h"
#include "generated/token.h"

typedef struct {
	LIST* tokens;
	TOKEN* current;
	TOKEN* prev;
	TOKEN* next;
	int index;
} PARSER;

typedef enum {
	TYPE_DEF,
	BASIC,
	UNARY,
	TERM,
} AST_EXPR_TYPE;

typedef struct {
	AST_EXPR_TYPE type;
	TOKEN_TYPE op;
	LIST* tokens;
	LIST* children;
} AST_EXPR;

PARSER* parser_init(LIST* tokens);
void free_parser(PARSER* parser);

TOKEN* parser_advance(PARSER* parser);
TOKEN* parser_match(PARSER* parser, TOKEN_TYPE);
void parser_eat(PARSER* parser, TOKEN_TYPE);
bool parser_is_at_end(PARSER* parser);
void parser_parse(PARSER* parser);

AST_EXPR* ast_expr_init(AST_EXPR_TYPE type, TOKEN_TYPE op, LIST* tokens, LIST* children);
void ast_expr_print(AST_EXPR* expr);

void expression_statement(PARSER* parser);
AST_EXPR* expression(PARSER* parser);
AST_EXPR* term(PARSER* parser);
AST_EXPR* unary(PARSER* parser);
AST_EXPR* static_type(PARSER* parser);
AST_EXPR* basic(PARSER* parser);
#endif

