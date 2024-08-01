#include "parser.h"
#include "../common/list.h"
#include <stdlib.h>
#include <stdio.h>

PARSER* parser_init(LIST* tokens) {
	PARSER* parser = malloc(sizeof(PARSER));
	if (parser == NULL) {
		// TODO: Fix this when we add error handling
		printf("Could not init parser\n");
		exit(1);
	}
	parser->tokens = tokens;
	parser->index = 0;
	parser->prev = list_get(tokens, parser->index);
	parser->current = list_get(tokens, parser->index);
	parser->next = list_get(tokens, parser->index + 1);
	return parser;
}

TOKEN* parser_advance(PARSER* parser) {
	if (parser_is_at_end(parser)) {
		return NULL;
	}

	parser->prev = list_get(parser->tokens, parser->index);

	if (parser->tokens->size - parser->index == 2) {
		parser->current = list_get(parser->tokens, parser->index + 1);
		parser->next = list_get(parser->tokens, parser->index + 1);
		parser->index++;
		return parser->prev;
	}

	parser->current = list_get(parser->tokens, parser->index + 1);
	parser->next = list_get(parser->tokens, parser->index + 2);
	parser->index++;
	return parser->prev;
}

TOKEN* parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return parser->current;
	}
	return NULL;
}

void parser_eat(PARSER* parser, TOKEN_TYPE type) {
	if(parser_match(parser, type) == NULL) {
		// TODO: Fix this when we introduce error handling
		printf("Expected LINE_TERM\n");
		exit(1);
	}
}

bool parser_is_at_end(PARSER* parser) {
	return parser->tokens->size - parser->index == 1;
}

void parser_parse(PARSER* parser) {
	while(!parser_is_at_end(parser)) {
		expression_statement(parser);
	}
}

AST_EXPR* ast_expr_init(AST_EXPR_TYPE type, TOKEN_TYPE op, LIST* tokens, LIST* children) {
	AST_EXPR* expr = malloc(sizeof(AST_EXPR));
	expr->type = type;
	expr->tokens = tokens;
	expr->children = children;
	expr->op = op;
	return expr;
}

void ast_expr_print(AST_EXPR* expr) {
	printf("(");
	for (int i = 0; i < expr->tokens->size; i++) {
		printf("%s", ((TOKEN*)list_get(expr->tokens, i))->lexeme);
	}
	if (expr->children != NULL) {
		for (int i = 0; i < expr->children->size; i++) {
			ast_expr_print(list_get(expr->children, i));
		}
	}
	printf(")");
}

void expression_statement(PARSER* parser) {
	AST_EXPR* expr = expression(parser);
	parser_eat(parser, LINE_TERM);
	ast_expr_print(expr);
	printf("\n");
}

AST_EXPR* expression(PARSER* parser) {
	return term(parser);
}

AST_EXPR* term(PARSER* parser) {
	AST_EXPR* left = unary(parser);
	TOKEN* op;

	if (parser_match(parser, OP_MULT) != NULL) {
		op = parser->prev;
	} else if (parser_match(parser, OP_DIV) != NULL) {
		op = parser->prev;
	} else {
		return left;
	}

	AST_EXPR* right = term(parser);
	LIST *tokens = init_list(sizeof(TOKEN *));
	list_push(tokens, op);

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(UNARY, op->type, tokens, children);
	return expr;
}

AST_EXPR* unary(PARSER *parser) {
	TOKEN* matched = parser_match(parser, OP_MINUS);
	if (matched != NULL) {
		TOKEN *op = parser->prev;
		LIST *tokens = init_list(sizeof(TOKEN *));
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, basic(parser));
		list_push(tokens, op);
		AST_EXPR *expr = ast_expr_init(UNARY, OP_MINUS, tokens, children);
		return expr;
	}
	return static_type(parser);
}

AST_EXPR* static_type(PARSER* parser) {
	if (parser->current->type == IDENTIFIER && parser->next->type == COLON) {
		LIST *tokens = init_list(sizeof(TOKEN *));
		list_push(tokens, parser_advance(parser));
		list_push(tokens, parser_advance(parser));
		TOKEN* last = parser_match(parser, STATIC_TYPE);
		if (last != NULL) {
			list_push(tokens, parser->prev);
			// printf("Matched type definiton\n");
			return ast_expr_init(TYPE_DEF, NOP, tokens, NULL);
		}
		// TODO: Error here when we have errors
		printf("Incorrect static type\n");
		exit(1);
	}
	return basic(parser);
}

AST_EXPR* basic(PARSER* parser) {

	TOKEN* token = parser_match(parser, LITERAL);
	if (token != NULL) {
		LIST* tokens = init_list(sizeof(TOKEN*));
		// printf("Matched literal\n");
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, NOP, tokens, NULL);
	}

	token = parser_match(parser, IDENTIFIER);
	if (token != NULL) {
		LIST* tokens = init_list(sizeof(TOKEN*));
		// printf("Matched identifier\n");
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, NOP, tokens, NULL);
	}

	printf("Matched nothing!\n");
	// TODO: Handle this when we do error handling
	exit(1);
}

void free_parser(PARSER* parser) {

}

