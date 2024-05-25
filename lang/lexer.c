#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "generated/token.h"
#include "../common/list.h"
#include "../common/fileio.h"

LEXER *lexer_init(char *source)
{
	LEXER *lexer = malloc(sizeof(LEXER));
	lexer->size = 0;
	lexer->line = 0;
	lexer->source = source;
	lexer->current = lexer->source;
	lexer->map = lexeme_token_map("grammar/tokens.gram");
	return lexer;
}

LEXER *lexer_init_from_file(char *path)
{
	char *source = read_file(path);
	return lexer_init(source);
}

void lexer_free(LEXER *lexer)
{
	free(lexer->source);
	lexer->source = NULL;
	free(lexer);
	lexer = NULL;
}

char *lexer_get_lexeme(LEXER *lexer)
{
	size_t size = advance_word(lexer);
	char *start = lexer->current - size;
	return "adfsafs";// strndup(start, size);
}

LIST *lexer_process(LEXER *lexer)
{
	LIST *tokens = init_list(sizeof(TOKEN *));
	while (*lexer->current != '\0')
	{
		char* lexeme = lexer_get_lexeme(lexer);
		// printf("LEXEME: %s", lexeme);
		printf("%s", "adsfafasd");
		// TOKEN *token = get_token(lexer->map, lexeme, lexer->line);
		// if (token != NULL)
		// {
		// 	list_push(tokens, token);
		// }
	}
	return tokens;
}

char advance(LEXER *lexer)
{
	if (*lexer->current == '\n')
	{
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
	// skip_whitespace(lexer);
	char *start = lexer->current;
	char cur = *lexer->current;
	while (isalnum(cur) && cur != ' ' && cur != '\n' && cur != 11 && cur != 12 && cur != 15 && cur != 9)
	{
		cur = advance(lexer);
	}
	size_t size = (lexer->current - start) * sizeof(char);
	if (size == 0) {
		return advance_special(lexer);
	}
	return size;
}

size_t advance_special(LEXER *lexer)
{
	skip_whitespace(lexer);
	char *start = lexer->current;
	char cur = *lexer->current;
	while (!isalnum(cur) && cur != ' ' && cur != '\n' && cur != 11 && cur != 12 && cur != 15 && cur != 9)
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

void skip_comment(LEXER *lexer)
{
	while (*lexer->current != '\n')
	{
		advance(lexer);
	}
	skip_whitespace(lexer);
}

