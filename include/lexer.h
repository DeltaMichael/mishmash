#ifndef LEXER_H
#define LEXER_H
typedef struct lexer {
	char* source;
	char* current;
	size_t size;
} LEXER;

typedef enum {
	BEGIN,
	END,
	ASSIGN,
	LEFT_PAREN,
	RIGHT_PAREN,
	LINE_TERM,
	IDENTIFIER
} TOKEN_TYPE;

typedef struct {
	TOKEN_TYPE type;
	char* lexeme;
} TOKEN;

LEXER* init(char* source);
LEXER* init_from_file(char* path);
char advance(LEXER* lexer);
char peek(LEXER* lexer);
void free_lexer(LEXER* lexer);
TOKEN* get_token(LEXER* lexer);
char* read_file(char* path);

#endif

