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
	parser->has_error = 0;
	return parser;
}

AST_EXPR* init_ast_expr(TOKEN* op, AST_EXPR_TYPE type, LIST* children) {
	AST_EXPR* node = malloc(sizeof(AST_EXPR));
	node->op = op;
	node->children = children;
	node->type = type;
	return node;
}

AST_STMT* init_ast_stmt(AST_STMT_TYPE type, LIST* values, TOKEN* id) {
	AST_STMT* node = malloc(sizeof(AST_STMT));
	node->id = id;
	node->values = values;
	node->type = type;
	return node;
}

TOKEN* parser_previous(PARSER* parser) {
	if(parser->index > 0) {
		return parser->tokens->elements[parser->index - 1];
	}
	return parser->current;
}

TOKEN* parser_peek(PARSER* parser) {
	if(parser->index < parser->size - 1) {
		return parser->tokens->elements[parser->index + 1];
	}
	return parser->current;

}

void parser_eat(PARSER* parser, TOKEN_TYPE type, char* message) {
	if(!parser_match(parser, type)) {
		fprintf(stderr, "error: line %d: ", parser->current->line);
		fprintf(stderr, "%s\n", message);
		parser->has_error = 1;
	}
}

void parser_sync(PARSER* parser) {
	parser_advance(parser);
	while(!parser_is_at_end(parser)) {
		if(parser_previous(parser)->type == LINE_TERM) {
			return;
		}
		switch(parser_peek(parser)->type) {
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

void parser_advance(PARSER* parser) {
	parser->index++;
	if(parser->index < parser->size) {
		parser->current = parser->tokens->elements[parser->index];
	}
}

bool parser_is_at_end(PARSER* parser) {
	return parser->index >= parser->size;
}

bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

AST_STMT* statement(PARSER* parser) {
	return block(parser);
}

AST_STMT* block(PARSER* parser) {
	if(parser_match(parser, DECLR)) {
		// parse declarations
	}
	if(parser_match(parser, BEGIN)) {
		LIST* values = init_list(sizeof(AST_STMT*));
		while(parser_peek(parser)->type != END) {
			AST_STMT* stmt = statement(parser);
			list_push(values, stmt);
		}
		AST_STMT* stmt = init_ast_stmt(BLOCK, values, NULL);
		parser_eat(parser, END, "Expected 'END' at the end of block");
		return stmt;
	}
	return assignment(parser);
}

AST_STMT* assignment(PARSER* parser) {
	if(parser->current->type == IDENTIFIER && parser_peek(parser)->type == ASSIGN) {
		parser_match(parser, IDENTIFIER);
		TOKEN* id = parser_previous(parser);
		parser_match(parser, ASSIGN);
		AST_STMT* assigned = statement(parser);
		LIST* values = init_list(sizeof(AST_STMT*));
		list_push(values, assigned);
		AST_STMT* stmt = init_ast_stmt(ASSIGNMENT, values, id);
		return stmt;
	}
	return expression_statement(parser);
}

AST_STMT* expression_statement(PARSER* parser) {
	LIST* values = init_list(sizeof(AST_EXPR*));
	list_push(values, expression(parser));
	AST_STMT* stmt = init_ast_stmt(EXPRESSION_STATEMENT, values, NULL);
	parser_eat(parser, LINE_TERM, "Expected line terminator at end of line");
	return stmt;
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
	AST_EXPR* left = unary(parser);
	AST_EXPR* right;
	while (parser_match(parser, MULT) || parser_match(parser, DIV)) {
		TOKEN* op = parser_previous(parser);
		right = unary(parser);
		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);
		left = init_ast_expr(op, BINARY, children);
	}
	return left;

}

AST_EXPR* unary(PARSER* parser) {
	if (parser_match(parser, MINUS)) {
		TOKEN* op = parser_previous(parser);
		AST_EXPR* right = unary(parser);
		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, right);
		return init_ast_expr(op, UNARY, children);
	}
	return primary(parser);
}

AST_EXPR* primary(PARSER* parser) {
	if(parser_match(parser, INT_LITERAL) || parser_match(parser, IDENTIFIER)) {
		return init_ast_expr(parser_previous(parser), PRIMARY, NULL);
	}
	if(parser_match(parser, LEFT_PAREN)) {
		AST_EXPR* expr = expression(parser);
		parser_eat(parser, RIGHT_PAREN, "Expected closing ')'");
		return expr;
	}
	// TODO: Error handling and synchronization
	// Print all errors, not just the first one
	fprintf(stderr, "Unknown primary token %s", parser->current->lexeme);
	exit(1);
}

