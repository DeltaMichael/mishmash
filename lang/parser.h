#ifndef PARSER_H
#define PARSER_H

#include "../common/list.h"
#include "generated/token.h"

typedef struct {
	LIST* tokens;
	TOKEN* current;
	TOKEN* prev;
	TOKEN* next;
	int index;
} PARSER;

PARSER* parser_init(LIST* tokens);
void free_parser(PARSER* parser);

void parser_advance(PARSER* parser);
bool parser_match(PARSER* parser, TOKEN_TYPE);
void parser_eat(PARSER* parser, TOKEN_TYPE);
bool parser_is_at_end(PARSER* parser);
void parser_parse(PARSER* parser);

void expression_statement(PARSER* parser);
void expression(PARSER* parser);
void static_type(PARSER* parser);
void basic(PARSER* parser);
#endif

