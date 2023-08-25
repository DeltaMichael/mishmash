#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"

LEXER *init(char *source)
{
	LEXER *lexer = malloc(sizeof(LEXER));
	lexer->size = 0;
	lexer->source = source;
	lexer->current = lexer->source;
	return lexer;
}

LEXER *init_from_file(char *path)
{
	char *source = read_file(path);
	return init(source);
}

char advance(LEXER *lexer)
{
	if (*lexer->current != '\0')
	{
		lexer->current += sizeof(char);
	}
	return *lexer->current;
}

char peek(LEXER *lexer)
{
	return *(lexer->current + sizeof(char));
}

void skip_whitespace(LEXER *lexer)
{
	char val = *lexer->current;
	while (val == ' ' || val == '\n' || val == 11 || val == 12 || val == 15 || val == 9)
	{
		val = advance(lexer);
	}
}

TOKEN *get_token(LEXER *lexer)
{
	TOKEN *token = malloc(sizeof(TOKEN));
	char *start = lexer->current;
	char cur = *lexer->current;
	while (isalnum(cur) && cur != ' ' && cur != '\n' && cur != 11 && cur != 12 && cur != 15)
	{
		cur = advance(lexer);
	}
	size_t size = (lexer->current - start) * sizeof(char);
	if (size > 0)
	{
		token->type = IDENTIFIER;
		token->lexeme = strndup(start, size);
	}
	else
	{
		token->lexeme = strndup(start, 1);
		switch (*token->lexeme)
		{
		case ')':
			token->type = RIGHT_PAREN;
		case '(':
			token->type = LEFT_PAREN;
		default:
			token->type = IDENTIFIER;
		}
		advance(lexer);
	}
	skip_whitespace(lexer);
	return token;
}

void free_lexer(LEXER *lexer)
{
	free(lexer->source);
	lexer->current = NULL;
	free(lexer);
}

char *read_file(char *path)
{
	FILE *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);
	rewind(f);
	char *out = malloc(file_size + 1);
	fread(out, sizeof(char), file_size, f);
	out[file_size] = 0;
	return out;
}
