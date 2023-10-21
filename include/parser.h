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
	bool exited_with_error;
	size_t size;
	int index;
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

typedef enum {
	ASSIGNMENT,
	DECLARATION,
	BLOCK,
	PRINT_STATEMENT,
	EXPRESSION_STATEMENT
} AST_STMT_TYPE;

typedef struct {
	TOKEN* id;
	AST_STMT_TYPE type;
	LIST* values;
} AST_STMT;

// Init functions
PARSER* init_parser(LIST* tokens);
void free_parser(PARSER* parser);

AST_EXPR* init_ast_expr(TOKEN* op, AST_EXPR_TYPE type, LIST* children);
AST_STMT* init_ast_stmt(AST_STMT_TYPE type, LIST* values, TOKEN* id);

// Parser navigation functions
void parser_advance(PARSER* parser);
TOKEN* parser_peek(PARSER* parser);
TOKEN* parser_previous(PARSER* parser);
void parser_error(PARSER* parser, char* message);
bool parser_match(PARSER* parser, TOKEN_TYPE type);
void parser_eat(PARSER* parser, TOKEN_TYPE type, char* message);
bool parser_is_at_end(PARSER* parser);
void parser_sync(PARSER* parser);

// Entry point
AST_STMT* parser_parse(PARSER* parser);

// Syntax tree building functions
AST_STMT* statement(PARSER* parser);
AST_STMT* block(PARSER* parser);
AST_STMT* block_body(PARSER* parser, LIST* values);
AST_STMT* print_stmt(PARSER* parser);
AST_STMT* declaration(PARSER* parser);
AST_STMT* assignment(PARSER* parser);
AST_STMT* expression_statement(PARSER* parser);
AST_EXPR* expression(PARSER* parser);
AST_EXPR* equality(PARSER* parser);
AST_EXPR* term(PARSER* parser);
AST_EXPR* factor(PARSER* parser);
AST_EXPR* unary(PARSER* parser);
AST_EXPR* primary(PARSER* parser);
#endif

