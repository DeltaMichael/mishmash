#ifndef PARSER_H
#define PARSER_H

#include "../common/list.h"
#include "lexer.h"

typedef struct {
	LIST* tokens;
	TOKEN* current;
	TOKEN* prev;
	TOKEN* next;
	int index;
} PARSER;

typedef enum {
	R_OR,
	R_AND,
	R_ZERO_OR_MORE,
	R_ONE_OR_MORE,
	R_ZERO_OR_ONE,
	R_ONE_OF,
	R_LEAF
} RULE_OP;

typedef struct {
	RULE_OP op;
	TOKEN* token;
	LIST* children;
} RULE;

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

RULE* line(PARSER* parser);
RULE* basic(PARSER* parser);
RULE* add(PARSER* parser);

RULE* init_rule(RULE_OP op, LIST* children, TOKEN* token);
void print_rule(RULE* rule);
void gen_code(RULE* rule);
LIST* split_children(RULE* rule);
#endif

