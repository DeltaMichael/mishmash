#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

H_MAP *lexeme_token_map(char* token_file_path) {
	H_MAP *map = init_hashmap();
	return map;
}

void free_token(TOKEN *token)
{
	if (token == NULL)
	{
		return;
	}
	if (token->lexeme != NULL)
	{
		free(token->lexeme);
		token->lexeme = NULL;
	}
	free(token);
	token = NULL;
}

void free_token_list(LIST *list)
{
	for(int i = 0; i < list->size; i++) {
		free_token(list->elements[i]);
	}
	free(list->elements);
	list->elements = NULL;
	free(list);
	list = NULL;
}

TOKEN *get_token(H_MAP *token_map, char* lexeme, int line)
{
	TOKEN *token = malloc(sizeof(TOKEN));
	token->lexeme = lexeme;
	if (contains_key(token_map, token->lexeme)) {
		token->type = *(TOKEN_TYPE*)hashmap_get(token_map, token->lexeme);
	} else if (isalpha(*token->lexeme)) {
		token->type = IDENTIFIER;
	} else if (isdigit(*token->lexeme)) {
		token->type = LITERAL;
	}
	else
	{
		printf("Lexeme %s could not be classified as anything",
				token->lexeme);
		// exit(1);
	}
	return token;
}

