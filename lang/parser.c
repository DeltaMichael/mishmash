#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

PARSER* parser_init(LIST* tokens) {
	PARSER* parser = malloc(sizeof(PARSER));
	if (parser == NULL) {
		// TODO: Fix this when we add error handling
		exit(1);
	}
	parser->tokens = tokens;
	parser->index = 0;
	parser->prev = list_get(tokens, parser->index);
	parser->current = list_get(tokens, parser->index);
	parser->next = list_get(tokens, parser->index + 1);
	return parser;
}

void parser_advance(PARSER* parser) {
	if (parser_is_at_end(parser)) {
		return;
	}

	parser->prev = list_get(parser->tokens, parser->index);

	if (parser->tokens->size - parser->index == 2) {
		parser->current = list_get(parser->tokens, parser->index + 1);
		parser->next = list_get(parser->tokens, parser->index + 1);
		parser->index++;
		return;
	}

	parser->current = list_get(parser->tokens, parser->index + 1);
	parser->next = list_get(parser->tokens, parser->index + 2);
	parser->index++;
}

bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

void parser_eat(PARSER* parser, TOKEN_TYPE type) {
	if(!parser_match(parser, type)) {
		// TODO: Fix this when we introduce error handling
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

void expression_statement(PARSER* parser) {
	expression(parser);
	parser_eat(parser, LINE_TERM);
}

void expression(PARSER* parser) {
	static_type(parser);
}

void static_type(PARSER* parser) {
	if (parser->current->type == IDENTIFIER && parser->next->type == COLON) {
		parser_advance(parser);
		parser_advance(parser);
		if (parser_match(parser, STATIC_TYPE)) {
			printf("Matched type definiton\n");
			return;
		}
		// TODO: Error here when we have errors
		exit(1);
	}
	basic(parser);
}

void basic(PARSER* parser) {
	if (parser_match(parser, LITERAL)) {
		printf("Matched literal\n");
		return;
	}
	if (parser_match(parser, IDENTIFIER)) {
		printf("Matched identifier\n");
		return;
	}
	printf("Matched nothing!\n");
}

void free_parser(PARSER* parser) {

}

