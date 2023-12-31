#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>

typedef struct lexer
{
	char *source;
	char *current;
	size_t size;
	int line;
} LEXER;

typedef enum
{
	// KEYWORDS
	FUN,
	BEGIN,
	END,
	DECLR,
	RETRUN,
	// ONE-CHAR SYMBOLS
	COLON,
	LEFT_PAREN,
	RIGHT_PAREN,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	COMMA,
	LINE_TERM,
	// OPERATORS
	PRINT_OP,
	ASSIGN,
	EQUALS,
	PLUS,
	MINUS,
	MULT,
	DIV,
	STATIC_TYPE,

	// PRIMARY TOKENS
	IDENTIFIER,
	INT_LITERAL
} TOKEN_TYPE;

typedef struct
{
	TOKEN_TYPE type;
	char *lexeme;
	int line;
} TOKEN;

LEXER *init(char *source);
LEXER *init_from_file(char *path);
char advance(LEXER *lexer);
size_t advance_word(LEXER *lexer);
char peek(LEXER *lexer);
void free_lexer(LEXER *lexer);
TOKEN *get_token(LEXER *lexer);
char *read_file(char *path);

#endif

