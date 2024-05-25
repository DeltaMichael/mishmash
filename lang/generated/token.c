#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

H_MAP *lexeme_token_map(char* token_file_path) {
	H_MAP *map = init_hashmap();

	TOKEN_TYPE *type = malloc(sizeof(TOKEN_TYPE*));
	*type = COND_START;
	hashmap_put(map, "if", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = COND_ALT;
	hashmap_put(map, "else", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = BLOCK_DECLR;
	hashmap_put(map, "declr", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = BLOCK_START;
	hashmap_put(map, "begin", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = BLOCK_END;
	hashmap_put(map, "end", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_ASSIGN;
	hashmap_put(map, ":=", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_PLUS;
	hashmap_put(map, "+", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_MINUS;
	hashmap_put(map, "-", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_MULT;
	hashmap_put(map, "*", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_DIV;
	hashmap_put(map, "/", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_LT;
	hashmap_put(map, "<", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_GT;
	hashmap_put(map, ">", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_LTE;
	hashmap_put(map, "<=", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_GTE;
	hashmap_put(map, ">=", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_EQ;
	hashmap_put(map, "=", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_NEQ;
	hashmap_put(map, "!=", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_BOOL_MULT;
	hashmap_put(map, "and", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_BOOL_PLUS;
	hashmap_put(map, "or", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_PRINT;
	hashmap_put(map, "print", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = OP_BOOL_NOT;
	hashmap_put(map, "!", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = LINE_TERM;
	hashmap_put(map, ";", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = DELIMITER;
	hashmap_put(map, ",", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = LEFT_BRACE;
	hashmap_put(map, "(", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = RIGHT_BRACE;
	hashmap_put(map, ")", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = THIN_ARROW;
	hashmap_put(map, "->", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = QUOTATION_MARK;
	hashmap_put(map, "\"", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = COLON;
	hashmap_put(map, ":", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	*type = STATIC_TYPE;
	hashmap_put(map, "s8", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "s16", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "s32", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "s64", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "u8", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "u16", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "u32", type);

	type = malloc(sizeof(TOKEN_TYPE*));
	hashmap_put(map, "u64", type);

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

