#include "parser.h"
#include "../common/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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

bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

bool parser_match_one_of(PARSER* parser, int count, ...) {
	va_list tokens;
	va_start(tokens, count);
	for (int i = 0; i < count; i++) {
		TOKEN_TYPE type = va_arg(tokens, TOKEN_TYPE);
		if (parser_match(parser, type)) {
			va_end(tokens);
			return true;
		}
	}
	va_end(tokens);
	return false;
}

bool parser_match_all(PARSER* parser, int count, ...) {
	va_list tokens;
	va_start(tokens, count);
	int index = parser->index;
	if(parser->tokens->size - (index + count) < 1) {
		return false;
	}
	for (int i = 0; i < count; i++) {
		TOKEN_TYPE type = va_arg(tokens, TOKEN_TYPE);
		TOKEN* token = list_get(parser->tokens, index);
		if (token->type != type) {
			return false;
		}
		index++;
	}
	parser->index = index - 1;
	parser_advance(parser);
	va_end(tokens);
	return true;
}

LIST* parser_get_prev(PARSER* parser, int count) {
	LIST* tokens = init_list(sizeof(TOKEN*));
	int token_count = count;
	if (parser->index + 1 < count) {
		token_count = parser->index + 1;
	}
	for (int i = token_count; i >= 0; i--) {
		list_push(tokens, list_get(parser->tokens, parser->index - i));
	}
	return tokens;
}

void parser_eat(PARSER* parser, TOKEN_TYPE type) {
	if(!parser_match(parser, type)) {
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
		ast_expr_print(line(parser));
		printf("\n");
	}
}

AST_EXPR* ast_expr_init(AST_EXPR_TYPE type, TOKEN_TYPE op, LIST* tokens, LIST* children) {
	AST_EXPR* expr = malloc(sizeof(AST_EXPR));
	expr->type = type;
	expr->tokens = tokens;
	expr->children = children;
	expr->op = op;
	expr->is_terminated = false;
	return expr;
}

void ast_expr_print(AST_EXPR* expr) {
	printf("[");
	for (int i = 0; i < expr->tokens->size; i++) {
		printf("%s", ((TOKEN*)list_get(expr->tokens, i))->lexeme);
	}
	if (expr->children != NULL) {
		for (int i = 0; i < expr->children->size; i++) {
			ast_expr_print(list_get(expr->children, i));
		}
	}
	printf("]");
}

AST_EXPR* line(PARSER* parser) {
	AST_EXPR* expr = assignment(parser);
	expr->is_terminated = true;
	parser_eat(parser, LINE_TERM);
	return expr;
}

AST_EXPR* assignment(PARSER* parser) {
	AST_EXPR* expr = expression(parser);
	if (expr->type == BASIC && parser_match(parser, OP_ASSIGN)) {
		LIST *tokens = init_list(sizeof(TOKEN *));
		list_push(tokens, parser->prev);

		LIST* children = init_list(sizeof(AST_EXPR*));
		AST_EXPR* asignee = expression(parser);
		list_push(children, expr);
		list_push(children, asignee);

		AST_EXPR *expr = ast_expr_init(ASSIGNMENT, OP_ASSIGN, tokens, children);
		return expr;
	}
	return expr;
}

AST_EXPR* expression(PARSER* parser) {
	return comparison(parser);
}

AST_EXPR* comparison(PARSER* parser) {
	AST_EXPR* left = factor(parser);
	TOKEN* op;

	if (parser_match_one_of(parser, 6, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_EQ, OP_NEQ)) {
		op = parser->prev;
	} else {
		return left;
	}

	AST_EXPR* right = factor(parser);
	LIST *tokens = init_list(sizeof(TOKEN *));
	list_push(tokens, op);

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(COMPARISON, op->type, tokens, children);
	return expr;
}

AST_EXPR* factor(PARSER* parser) {
	AST_EXPR* left = term(parser);
	TOKEN* op;

	if (parser_match_one_of(parser, 2, OP_PLUS, OP_MINUS)) {
		op = parser->prev;
	} else {
		return left;
	}

	AST_EXPR* right = factor(parser);
	LIST *tokens = init_list(sizeof(TOKEN *));
	list_push(tokens, op);

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(FACTOR, op->type, tokens, children);
	return expr;
}

AST_EXPR* term(PARSER* parser) {
	AST_EXPR* left = unary(parser);
	TOKEN* op;

	if (parser_match_one_of(parser, 2, OP_MULT, OP_DIV)) {
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

	AST_EXPR *expr = ast_expr_init(TERM, op->type, tokens, children);
	return expr;
}

AST_EXPR* unary(PARSER *parser) {
	 if (parser_match(parser, OP_MINUS)) {
		TOKEN *op = parser->prev;
		LIST *tokens = init_list(sizeof(TOKEN *));
		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, basic(parser));
		list_push(tokens, op);
		AST_EXPR *expr = ast_expr_init(UNARY, OP_MINUS, tokens, children);
		return expr; }
	 return static_type(parser);
}

AST_EXPR* static_type(PARSER* parser) {
	if (parser_match_all(parser, 3, IDENTIFIER, COLON, STATIC_TYPE)) {
		LIST* tokens = parser_get_prev(parser, 3);
		return ast_expr_init(TYPE_DEF, NOP, tokens, NULL);
	}
	return basic(parser);
}

AST_EXPR* func_call(PARSER* parser) {
	if(parser_match_all(parser, 2, IDENTIFIER, LEFT_BRACE)) {
		LIST* children = init_list(sizeof(AST_EXPR*));
		LIST* tokens = parser_get_prev(parser, 2);
		AST_EXPR* expr = expression(parser);

		if(expr == NULL) {
			parser_eat(parser, RIGHT_BRACE);
			list_push(tokens, parser->prev);
			return ast_expr_init(BASIC, IDENTIFIER, tokens, children);
		}

		list_push(children, expr);
		while(parser_match(parser, DELIMITER)) {
			AST_EXPR* expr = expression(parser);
			list_push(children, expr);
		}
		parser_eat(parser, RIGHT_BRACE);
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, IDENTIFIER, tokens, children);
	}
	return NULL;
}

AST_EXPR* identifier(PARSER* parser) {
	if(parser_match(parser, IDENTIFIER)) {
		LIST* tokens = init_list(sizeof(TOKEN*));
		// printf("Matched identifier\n");
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, NOP, tokens, NULL);
	}
	return NULL;
}

AST_EXPR* basic(PARSER* parser) {

	if(parser_match(parser, LEFT_BRACE)) {
		AST_EXPR* expr = expression(parser);
		parser_eat(parser, RIGHT_BRACE);
		LIST *children = init_list(sizeof(AST_EXPR *));
		LIST *tokens = init_list(sizeof(AST_EXPR *));
		list_push(children, expr);
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, OP_MINUS, tokens, children);
	}

	AST_EXPR* expr = func_call(parser);
	if(expr != NULL) {
		return expr;
	}

	expr = identifier(parser);
	if(expr != NULL) {
		return expr;
	}

	if(parser_match(parser, LITERAL)) {
		LIST* tokens = init_list(sizeof(TOKEN*));
		list_push(tokens, parser->prev);
		return ast_expr_init(BASIC, NOP, tokens, NULL);
	}

	// TODO: Handle this when we do error handling
	return NULL;
}

void free_parser(PARSER* parser) {

}

