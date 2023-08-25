#ifndef LEXER_H
#define LEXER_H
typedef struct lexer {
	char* source;
	char* current;
	size_t size;
} LEXER;

LEXER* init(char* source);
char advance(LEXER* lexer);
char peek(LEXER* lexer);
void free_lexer(LEXER* lexer);

#endif

