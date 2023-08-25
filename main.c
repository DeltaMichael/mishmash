#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"

typedef enum {
	BEGIN,
	END,
	ASSIGN,
	LEFT_PAREN,
	RIGHT_PAREN,
	IDENTIFIER
} TOKEN_TYPE;

typedef struct {
	TOKEN_TYPE type;
	char* lexeme;
} TOKEN;

TOKEN* get_token(LEXER* lexer);

int main(int argc, char** argv) {
	char* str = "my first string that)s gonna be parsed";
	LEXER* lexer = init(str);
	while(*lexer->current != '\0') {
		TOKEN* token = get_token(lexer);
		printf("%s\n", token->lexeme);
	}
	free(lexer->source);
	free(lexer);
}

LEXER* init(char* source) {
	LEXER* lexer = malloc(sizeof(LEXER));
	lexer->size = strlen(source);
	lexer->source = malloc(lexer->size * sizeof(char));
	lexer->current = lexer->source;
	strcpy(lexer->source, source);
	return lexer;
}

char advance(LEXER* lexer) {
	if(*lexer->current != '\0') {
		lexer->current += sizeof(char);
	}
	return *lexer->current;
}

char peek(LEXER* lexer) {
	return *(lexer->current + sizeof(char));
}

void skip_whitespace(LEXER* lexer) {
	char val = *lexer->current;
	while(val == ' ' || val == '\n' || val == 11 || val == 12 || val == 15) {
		val = advance(lexer);
	}
}

TOKEN* get_token(LEXER* lexer) {
	TOKEN* token = malloc(sizeof(TOKEN));
	char* start = lexer->current;
	char cur = *lexer->current;
	while(isalnum(cur) && cur != ' ' && cur != '\n' && cur != 11 && cur != 12 && cur != 15) {
		cur = advance(lexer);
	}
	size_t size = (lexer->current - start) * sizeof(char);
	if(size > 0) {
		token->type = IDENTIFIER;
		token->lexeme = strndup(start, size);
		skip_whitespace(lexer);
	} else {
		token->lexeme = strndup(start, 1);
		switch(*token->lexeme) {
			case ')':
				token->type = RIGHT_PAREN;
			case '(':
				token->type = LEFT_PAREN;
			default:
				token->type = IDENTIFIER;
		}
		advance(lexer);
	}
	return token;
}

void free_lexer(LEXER* lexer) {
	free(lexer->source);
	lexer->current = NULL;
	free(lexer);
}

