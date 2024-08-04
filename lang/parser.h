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

TOKEN* parser_advance(PARSER* parser);
bool parser_match(PARSER* parser, TOKEN_TYPE);
bool parser_match_all(PARSER* parser, int count, ...);
bool parser_match_one_of(PARSER* parser, int count, ...);
LIST* parser_get_prev(PARSER* parser, int count);
void parser_eat(PARSER* parser, TOKEN_TYPE);
bool parser_is_at_end(PARSER* parser);
void parser_parse(PARSER* parser);

#endif

