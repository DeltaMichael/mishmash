#ifndef LEXER_H
#define LEXER_H

#include "../common/list.h"
#include "../common/hashmap.h"

typedef struct
{
	char *source;
	char *current;
	size_t size;
	int line;
	H_MAP *map;
} LEXER;

typedef enum {
	T_RULE_NAME,
	T_TERMINAL,
	T_NONTERMINAL,
	T_OR,
	T_STAR,
	T_PLUS,
	T_QMARK,
	T_LEFT_BRACE,
	T_RIGHT_BRACE,
	T_DOLLAR_SIGN,
	T_LINE_TERM
} TOKEN_TYPE;

typedef struct {
	char* lexeme;
	int line;
	TOKEN_TYPE type;
} TOKEN;

LEXER *lexer_init(char *source);
LEXER *lexer_init_from_file(char *path);
void lexer_free(LEXER *lexer);
char *lexer_get_lexeme(LEXER *lexer);
LIST *lexer_process(LEXER *lexer);
char advance(LEXER *lexer);
size_t advance_word(LEXER *lexer);
size_t advance_special(LEXER *lexer);
char peek(LEXER *lexer);
void skip_whitespace(LEXER *lexer);
void skip_comment(LEXER *lexer);
bool is_at_end(LEXER *lexer);
bool is_token(LEXER *lexer, char* start);
TOKEN *get_token(H_MAP *token_map, char* lexeme, int line);
bool is_terminal(char* in);
bool is_name(char* in);
TOKEN* clone_token(TOKEN* token);
#endif

