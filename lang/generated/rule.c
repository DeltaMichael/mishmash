#include "rule.h"
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
		char* op = parser->prev->lexeme;

		LIST* children = init_list(sizeof(AST_EXPR*));
		AST_EXPR* asignee = expression(parser);
		list_push(children, expr);
		list_push(children, asignee);

		AST_EXPR *expr = ast_expr_init(ASSIGNMENT, OP_ASSIGN, op, children);
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

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(COMPARISON, op->type, op->lexeme, children);
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

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(FACTOR, op->type, op->lexeme, children);
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

	LIST* children = init_list(sizeof(AST_EXPR*));
	list_push(children, left);
	list_push(children, right);

	AST_EXPR *expr = ast_expr_init(TERM, op->type, op->lexeme, children);
	return expr;
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
		LIST* tokens = parser_get_prev(parser, 3);
		char* op = concat_lexemes(tokens);
		return ast_expr_init(TYPE_DEF, NOP, op, NULL);
	}
	return basic(parser);
}

char* concat_lexemes(LIST* tokens) {
	int size = 0;
	for (int i = 0; i < tokens->size; i++) {
		size += strlen(((TOKEN*)(list_get(tokens, i)))->lexeme);
	}
	char *op = malloc(size * sizeof(char) + 1);

	strcpy(op, ((TOKEN*)(list_get(tokens, 0)))->lexeme);
	for (int i = 1; i < tokens->size; i++) {
		strcat(op, ((TOKEN*)(list_get(tokens, i)))->lexeme);
	}
	return op;
}

AST_EXPR* func_call(PARSER* parser) {
	if(parser_match_all(parser, 2, IDENTIFIER, LEFT_BRACE)) {
		LIST* children = init_list(sizeof(AST_EXPR*));

		LIST* tokens = parser_get_prev(parser, 2);

		AST_EXPR* expr = expression(parser);

		if(expr == NULL) {
			parser_eat(parser, RIGHT_BRACE);
			list_push(tokens, parser->prev);
			char *op = concat_lexemes(tokens);
			return ast_expr_init(BASIC, IDENTIFIER, op, children);
		}

		list_push(children, expr);
		while(parser_match(parser, DELIMITER)) {
			AST_EXPR* expr = expression(parser);
			list_push(children, expr);
		}
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
		return ast_expr_init(BASIC, NOP, op->lexeme, NULL);
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
		return ast_expr_init(BASIC, NOP, op->lexeme, children);
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
		return ast_expr_init(BASIC, NOP, op->lexeme, NULL);
	}

	// TODO: Handle this when we do error handling
	return NULL;
}

