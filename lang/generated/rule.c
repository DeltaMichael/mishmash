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
	if (expression(parser) && parser_match(parser, OP_ASSIGN)) {
		TOKEN* op = parser->prev;
		AST_EXPR *expr = ast_expr_init(ASSIGNMENT, op->type, op->lexeme);
		list_push(expr->children, parser->prev_expr);
		expression(parser);
		list_push(expr->children, parser->prev_expr);
	}
	return parser->prev_expr;
}

AST_EXPR* expression(PARSER* parser) {
	comparison(parser);
	return parser->prev_expr;
}

AST_EXPR* comparison(PARSER* parser) {
	if (factor(parser) && parser_match_one_of(parser, 6, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_EQ, OP_NEQ)) {
		TOKEN* op = parser->prev;
		AST_EXPR *expr = ast_expr_init(COMPARISON, op->type, op->lexeme);
		list_push(expr->children, parser->prev_expr);

		factor(parser);
		list_push(expr->children, parser->prev_expr);

		parser->prev_expr = expr;
	}

	return parser->prev_expr;
}

AST_EXPR* factor(PARSER* parser) {
	if (term(parser) && parser_match_one_of(parser, 2, OP_PLUS, OP_MINUS)) {
		TOKEN* op = parser->prev;
		AST_EXPR *expr = ast_expr_init(FACTOR, op->type, op->lexeme);
		list_push(expr->children, parser->prev_expr);

		term(parser);
		list_push(expr->children, parser->prev_expr);

		parser->prev_expr = expr;
	}

	return parser->prev_expr;

}

AST_EXPR* term(PARSER* parser) {
	if (unary(parser) && parser_match_one_of(parser, 2, OP_MULT, OP_DIV)) {
		TOKEN* op = parser->prev;
		AST_EXPR *expr = ast_expr_init(TERM, op->type, op->lexeme);
		list_push(expr->children, parser->prev_expr);

		term(parser);
		list_push(expr->children, parser->prev_expr);

		parser->prev_expr = expr;
	}

	return parser->prev_expr;
}

AST_EXPR* unary(PARSER *parser) {
	 if (parser_match(parser, OP_MINUS)) {
		TOKEN *op = parser->prev;

		AST_EXPR* expr = ast_expr_init(UNARY, op->type, op->lexeme);
		basic(parser);
		list_push(expr->children, parser->prev_expr);
		parser->prev_expr = expr;
	 } else {
	 	var_declr(parser);
	 }
	 return parser->prev_expr;
}

AST_EXPR* var_declr(PARSER* parser) {
	if (parser_match_all(parser, 3, IDENTIFIER, COLON, STATIC_TYPE)) {
		TOKEN_TYPE op_type = parser->prev->type;
		LIST* tokens = parser_get_prev(parser, 3);
		char* op = concat_lexemes(tokens);
		parser->prev_expr = ast_expr_init(VAR_DECLR, op_type, op);
	} else {
		basic(parser);
	}
	return NULL;
}

AST_EXPR* func_call(PARSER* parser) {
	if(parser_match_all(parser, 2, IDENTIFIER, LEFT_BRACE)) {
		LIST* tokens = parser_get_prev(parser, 2);
		char *op = concat_lexemes(tokens);
		AST_EXPR* expr = ast_expr_init(FUNC_CALL, IDENTIFIER, op);

		while(expression(parser) && parser_match(parser, DELIMITER)) {
			list_push(expr->children, parser->prev_expr);
		}

		parser_eat(parser, RIGHT_BRACE);
		parser->prev_expr = expr;
	}
	return NULL;
}

AST_EXPR* basic(PARSER* parser) {
	if(parser_match(parser, LEFT_BRACE) && expression(parser) && parser_eat(parser, RIGHT_BRACE)) {
		TOKEN* op = parser->prev;
		AST_EXPR* expr = ast_expr_init(BASIC, op->type, op->lexeme);
		list_push(expr->children, parser->prev_expr);
		parser->prev_expr = expr;
	} else if(func_call(parser)) {

	} else if(parser_match(parser, IDENTIFIER)) {
		TOKEN* op = parser->prev;
		parser->prev_expr = ast_expr_init(BASIC, op->type, op->lexeme);
	} else if(parser_match(parser, LITERAL)) {
		TOKEN* op = parser->prev;
		parser->prev_expr = ast_expr_init(BASIC, op->type, op->lexeme);
	}

	// TODO: Handle this when we do error handling
	return parser->prev_expr;
}

