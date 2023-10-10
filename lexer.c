#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"
#include "lexer.h"

LEXER *init(char *source)
{
	LEXER *lexer = malloc(sizeof(LEXER));
	lexer->size = 0;
	lexer->line = 0;
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
	if (*lexer->current == '\n') {
		lexer->line++;
	}

	if (*lexer->current != '\0')
	{
		lexer->current += sizeof(char);
	}
	return *lexer->current;
}

size_t advance_word(LEXER *lexer)
{
	char *start = lexer->current;
	char cur = *lexer->current;
	while (isalnum(cur) && cur != ' ' && cur != '\n' && cur != 11 && cur != 12 && cur != 15 && cur != 9)
	{
		cur = advance(lexer);
	}
	size_t size = (lexer->current - start) * sizeof(char);
	return size;
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

void skip_comment(LEXER *lexer) {
	while(*lexer->current != '\n') {
		advance(lexer);
	}
	skip_whitespace(lexer);
}

TOKEN *get_token(LEXER *lexer)
{
	TOKEN *token = malloc(sizeof(TOKEN));
	size_t size = advance_word(lexer);
	char *start = lexer->current - size;
	token->line = lexer->line;
	if (size > 0)
	{
		token->lexeme = strndup(start, size);
		if (strcmp(token->lexeme, "fun") == 0)
		{
			token->type = FUN;
		}
		else if (strcmp(token->lexeme, "DECLR") == 0)
		{
			token->type = DECLR;
		}
		else if (strcmp(token->lexeme, "BEGIN") == 0)
		{
			token->type = BEGIN;
		}
		else if (strcmp(token->lexeme, "END") == 0)
		{
			token->type = END;
		}
		else if (strcmp(token->lexeme, "print") == 0)
		{
			token->type = PRINT_OP;
		}
		else if (strcmp(token->lexeme, "int") == 0 || strcmp(token->lexeme, "string") == 0)
		{
			token->type = STATIC_TYPE;
		}
		else if(isalpha(*token->lexeme))
		{
			token->type = IDENTIFIER;

		}
		else if(isdigit(*token->lexeme))
		{
			token->type = INT_LITERAL;
		} else {
			printf("Lexeme %s could not be classified as anything", token->lexeme);
			exit(1);
		}
	}
	else
	{
		token->lexeme = strndup(start, 1);
		switch (*token->lexeme)
		{
		case ')':
			token->type = RIGHT_PAREN;
			break;
		case '(':
			token->type = LEFT_PAREN;
			break;
		case '[':
			token->type = LEFT_BRACKET;
			break;
		case ']':
			token->type = RIGHT_BRACKET;
			break;
		case ';':
			token->type = LINE_TERM;
			break;
		case ',':
			token->type = COMMA;
			break;
		case '=':
			token->type = EQUALS;
			break;
		case '+':
			token->type = PLUS;
			break;
		case '-':
			token->type = MINUS;
			break;
		case '*':
			token->type = MULT;
			break;
		case '/':
			if(peek(lexer) == '/') {
				skip_comment(lexer);
				return get_token(lexer);
			} else {
				token->type = DIV;
				break;
			}
		case ':':
			if (peek(lexer) == '=')
			{
				token->lexeme = strdup(":=");
				token->type = ASSIGN;
				advance(lexer);
			}
			else
			{
				token->type = COLON;
			}
			break;
		default:
			token->type = IDENTIFIER;
			break;
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

