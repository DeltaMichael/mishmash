#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"

LEXER* init(char* source) {
	LEXER* lexer = malloc(sizeof(LEXER));
	lexer->size = strlen(source);
	lexer->source = source;
	lexer->current = lexer->source;
	strcpy(lexer->source, source);
	return lexer;
}

LEXER* init_from_file(char* path) {
	char* source = read_file(path);
	return init(source);

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

char* read_file(char* path) {
	// TODO: Read file and allocate contents to heap
	return NULL;
}

