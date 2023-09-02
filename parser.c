#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "include/parser.h"
#include "include/list.h"

PARSER* init_parser(LIST* tokens) {
	PARSER* parser = malloc(sizeof(PARSER));
	parser->tokens = tokens;
	parser->index = 0;
	parser->current = (TOKEN*)tokens->elements[0];
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

TOKEN* parser_previous(PARSER* parser) {
	if(parser->index < parser->size) {
		return parser->tokens->elements[parser->index - 1];
	}
	return parser->current;
}

TOKEN* parser_peek(PARSER* parser) {
	return parser->current + sizeof(TOKEN*);
}
void parser_eat(PARSER* parser, TOKEN_TYPE type, char* message) {
	if(!parser_match(parser, type)) {
		printf("%s\n", message);
		exit(1);
	}
}

void parser_advance(PARSER* parser) {
	parser->index++;
	if(parser->index < parser->size) {
		parser->current = parser->tokens->elements[parser->index];
	}
}

bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

AST_EXPR* expression(PARSER* parser) {
	return equality(parser);
}

AST_EXPR* equality(PARSER* parser) {
	AST_EXPR* left = term(parser);
	AST_EXPR* right;
	while (parser_match(parser, EQUALS)) {
		TOKEN* op = parser_previous(parser);
		right = term(parser);
		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;
}

// TODO: Add comparison

AST_EXPR* term(PARSER* parser) {
	AST_EXPR* left = factor(parser);
	AST_EXPR* right;
	while (parser_match(parser, PLUS) || parser_match(parser, MINUS)) {
		TOKEN* op = parser_previous(parser);
		right = factor(parser);
		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;

}

AST_EXPR* factor(PARSER* parser) {
	AST_EXPR* left = primary(parser);
	AST_EXPR* right;
	while (parser_match(parser, MULT) || parser_match(parser, DIV)) {
		TOKEN* op = parser_previous(parser);
		right = primary(parser);
		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;

}

// TODO: Add unary minus

AST_EXPR* primary(PARSER* parser) {
	if(parser_match(parser, INT_LITERAL)) {
		return init_ast_expr(parser_previous(parser), PRIMARY, NULL);
	}
	if(parser_match(parser, LEFT_PAREN)) {
		AST_EXPR* expr = expression(parser);
		parser_eat(parser, RIGHT_PAREN, "Expected closing ')'");
		return expr;
	}
	exit(1);
}

AST_EXPR* comparison(PARSER* parser);

