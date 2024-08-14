#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "../common/list.h"
#include "../common/fileio.h"

LEXER *lexer_init(char *source)
{
	LEXER *lexer = malloc(sizeof(LEXER));
	lexer->size = 0;
	lexer->line = 0;
	lexer->source = source;
	lexer->current = lexer->source;

	lexer->map = init_hashmap();
	TOKEN_TYPE* type;

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_OR;
	hashmap_put(lexer->map, "|", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_STAR;
	hashmap_put(lexer->map, "*", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_PLUS;
	hashmap_put(lexer->map, "+", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_QMARK;
	hashmap_put(lexer->map, "?", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_LEFT_BRACE;
	hashmap_put(lexer->map, "(", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_RIGHT_BRACE;
	hashmap_put(lexer->map, ")", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = T_LINE_TERM;
	hashmap_put(lexer->map, ";", type);

	return lexer;
}

LEXER *lexer_init_from_file(char *path)
{
	char *source = read_file(path);
	return lexer_init(source);
}

void lexer_free(LEXER *lexer)
{
	free_hashmap(lexer->map);
	free(lexer->source);
	lexer->source = NULL;
	free(lexer);
	lexer = NULL;
}

char *lexer_get_lexeme(LEXER *lexer)
{
	size_t size = advance_word(lexer);
	char *start = lexer->current - size;
	return strndup(start, size);
}

LIST *lexer_process(LEXER *lexer)
{
	LIST *tokens = init_list(sizeof(TOKEN *));
	char *lexeme = lexer_get_lexeme(lexer);
	while(strcmp(lexeme, "\0") != 0) {
		if (lexeme[0] == '#') {
			skip_comment(lexer);
			lexeme = lexer_get_lexeme(lexer);
			continue;
		}
		TOKEN *token = get_token(lexer->map, lexeme, lexer->line);
		list_push(tokens, token);
		lexeme = lexer_get_lexeme(lexer);
	}
	free(lexeme);
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

bool is_token(LEXER *lexer, char* start) {
	size_t size = 1 + (lexer->current - start) * sizeof(char);
	char* lexeme = strndup(start, size);
	bool is_token = contains_key(lexer->map, lexeme);
	free(lexeme);
	return is_token;
}

bool is_accepted(char in) {
	return isalnum(in) || in == '_' || in == ':';
}

size_t advance_word(LEXER *lexer)
{
	skip_whitespace(lexer);
	char *start = lexer->current;
	char cur = *lexer->current;
	while (is_accepted(cur) && !(isspace(cur)) && !is_at_end(lexer) && !is_token(lexer, start))
	{
		cur = advance(lexer);
	}
	while(!isspace(cur) && !is_at_end(lexer) && is_token(lexer, start)) {
		cur = advance(lexer);
	}
	size_t size = (lexer->current - start) * sizeof(char);
	if (size == 0) {
		skip_whitespace(lexer);
		while (!isspace(cur) && !is_accepted(cur) && !is_at_end(lexer))
		{
			cur = advance(lexer);
		}
		size = (lexer->current - start) * sizeof(char);
	}
	return size;
}

void skip_whitespace(LEXER *lexer)
{
	char val = *lexer->current;
	while (isspace(val))
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

bool is_at_end(LEXER *lexer)
{
	return *lexer->current == '\0';
}

bool is_terminal(char* in) {
	while(*in != '\0') {
		if(islower(*in)) {
			return false;
		}
		in++;
	}
	return true;
}

bool is_name(char* in) {
	int len = strlen(in);
	if (in[len - 1] == ':') {
		in[len - 1] = '\0';
		return true;
	}
	return false;
}

TOKEN *get_token(H_MAP *token_map, char* lexeme, int line)
{
	TOKEN *token = malloc(sizeof(TOKEN));
	if (contains_key(token_map, lexeme)) {
		TOKEN_TYPE* type = hashmap_get(token_map, lexeme);
		token->type = *type;
	}
	else if (is_name(lexeme)) {
		token->type = T_RULE_NAME;
	} else if (is_terminal(lexeme)) {
		token->type = T_TERMINAL;
	} else {
		token->type = T_NONTERMINAL;
	}
	token->lexeme = lexeme;
	token->line = line;
	return token;
}

