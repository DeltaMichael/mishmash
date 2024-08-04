#include "parser.h"
#include "generated/rule.h"
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

void free_parser(PARSER* parser) {

}

