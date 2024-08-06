#include "rule.h"
#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

AST_EXPR* line(PARSER* parser) {
	AST_EXPR* expr = assignment(parser);
	expr->is_terminated = true;
	parser_eat(parser, LINE_TERM);
	return expr;
}

AST_EXPR* assignment(PARSER* parser) {
	AST_EXPR* expr = expression(parser);

	if (parser_match(parser, OP_ASSIGN)) {
		TOKEN* op = parser->prev;

		LIST* children = init_list(sizeof(AST_EXPR*));
		AST_EXPR* asignee = expression(parser);
		list_push(children, expr);
		list_push(children, asignee);

		AST_EXPR *expr = ast_expr_init(ASSIGNMENT, op->type, op->lexeme, children);
		return expr;
	}
	return expr;
}

AST_EXPR* expression(PARSER* parser) {
	return comparison(parser);
}

AST_EXPR* comparison(PARSER* parser) {
	AST_EXPR* left = factor(parser);

	if (parser_match_one_of(parser, 6, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_EQ, OP_NEQ)) {
		TOKEN* op = parser->prev;
		AST_EXPR* right = factor(parser);

		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);

		AST_EXPR *expr = ast_expr_init(COMPARISON, op->type, op->lexeme, children);
		return expr;
	}

	return left;
}

AST_EXPR* factor(PARSER* parser) {
	AST_EXPR* left = term(parser);

	if (parser_match_one_of(parser, 2, OP_PLUS, OP_MINUS)) {
		TOKEN* op = parser->prev;
		AST_EXPR* right = factor(parser);

		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);

		AST_EXPR *expr = ast_expr_init(FACTOR, op->type, op->lexeme, children);
		return expr;
	}
	return left;
}

AST_EXPR* term(PARSER* parser) {
	AST_EXPR* left = unary(parser);

	if (parser_match_one_of(parser, 2, OP_MULT, OP_DIV)) {
		TOKEN* op = parser->prev;
		AST_EXPR* right = term(parser);

		LIST* children = init_list(sizeof(AST_EXPR*));
		list_push(children, left);
		list_push(children, right);

		AST_EXPR *expr = ast_expr_init(TERM, op->type, op->lexeme, children);
		return expr;
	}

	return left;
}

AST_EXPR* unary(PARSER *parser) {
	 if (parser_match(parser, OP_MINUS)) {
		TOKEN *op = parser->prev;

		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, basic(parser));
		AST_EXPR *expr = ast_expr_init(UNARY, op->type, op->lexeme, children);
		return expr;
	 }
	 return static_type(parser);
}

AST_EXPR* static_type(PARSER* parser) {
	if (parser_match_all(parser, 3, IDENTIFIER, COLON, STATIC_TYPE)) {
		TOKEN_TYPE op_type = parser->prev->type;
		LIST* tokens = parser_get_prev(parser, 3);
		char* op = concat_lexemes(tokens);
		return ast_expr_init(TYPE_DEF, op_type, op, NULL);
	}
	return basic(parser);
}

AST_EXPR* func_call(PARSER* parser) {
	if(parser_match_all(parser, 2, IDENTIFIER, LEFT_BRACE)) {
		LIST* children = init_list(sizeof(AST_EXPR*));
		LIST* tokens = parser_get_prev(parser, 2);

		AST_EXPR* expr = NULL;
		do {
			expr = expression(parser);
			if (expr != NULL) list_push(children, expr);
		} while(expr != NULL && parser_match(parser, DELIMITER));

		parser_eat(parser, RIGHT_BRACE);

		list_push(tokens, parser->prev);
		char *op = concat_lexemes(tokens);

		return ast_expr_init(BASIC, IDENTIFIER, op, children);
	}
	return NULL;
}

AST_EXPR* identifier(PARSER* parser) {
	if(parser_match(parser, IDENTIFIER)) {
		TOKEN* op = parser->prev;
		return ast_expr_init(BASIC, op->type, op->lexeme, NULL);
	}
	return NULL;
}

AST_EXPR* basic(PARSER* parser) {

	if(parser_match(parser, LEFT_BRACE)) {
		TOKEN* op = parser->prev;
		AST_EXPR* expr = expression(parser);
		parser_eat(parser, RIGHT_BRACE);

		LIST *children = init_list(sizeof(AST_EXPR *));
		list_push(children, expr);
		return ast_expr_init(BASIC, op->type, op->lexeme, children);
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
		TOKEN* op = parser->prev;
		return ast_expr_init(BASIC, op->type, op->lexeme, NULL);
	}

	// TODO: Handle this when we do error handling
	return NULL;
}

