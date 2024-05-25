#ifndef LEXER_H
#define LEXER_H

#include "../common/list.h"
#include "../common/hashmap.h"

typedef struct lexer
{
	char *source;
	char *current;
	size_t size;
	int line;
	H_MAP *map;
} LEXER;

LEXER *lexer_init(char *source);
LEXER *lexer_init_from_file(char *path);
void lexer_free(LEXER *lexer);
char *lexer_get_lexeme(LEXER *lexer);
LIST *lexer_process(LEXER *lexer);
char advance(LEXER *lexer);
size_t advance_word(LEXER *lexer);
char peek(LEXER *lexer);
void skip_whitespace(LEXER *lexer);
void skip_comment(LEXER *lexer);
#endif

