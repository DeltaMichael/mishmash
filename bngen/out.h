#ifndef TOKEN_H
#define TOKEN_H
#include "../../common/hashmap.h"
#include "../../common/list.h"
typedef enum {
	IDENTIFIER,
	LITERAL
} TOKEN_TYPE;

typedef struct {
	char* lexeme;
	TOKEN_TYPE type;
	int line;
} TOKEN;

H_MAP *lexeme_token_map(char* token_file_path);
void free_token(TOKEN *token);
void free_token_list(LIST * list);
TOKEN *get_token(H_MAP *token_map, char* lexeme, int line);

#endif
