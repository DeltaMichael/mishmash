#include <stdlib.h>
#include <stdbool.h>
#include "include/parser.h"
#include "include/list.h"

PARSER* init_parser(LIST* tokens) {
	PARSER* parser = malloc(sizeof(PARSER));
	parser->tokens = tokens;
	parser->current = (TOKEN*)tokens->elements;
	parser->size = tokens->size;
	return parser;
}

AST_EXPR* init_ast_expr(TOKEN* op, AST_EXPR_TYPE type, LIST* children) {
	AST_EXPR* node = malloc(sizeof(AST_EXPR));
	node->op = op;
	node->children = children;
	node->type = type;
	return node;
}

TOKEN* parser_peek(PARSER* parser) {
	return NULL;
}
void parser_eat(PARSER* parser, TOKEN_TYPE type) {

}
bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	return true;
}

