#ifndef TOKEN_H
#define TOKEN_H
#include "../../common/hashmap.h"
#include "../../common/list.h"
typedef enum {
	COND_START,
	COND_ALT,
	BLOCK_DECLR,
	BLOCK_START,
	BLOCK_END,
	OP_ASSIGN,
	OP_PLUS,
	OP_MINUS,
	OP_MULT,
	OP_DIV,
	OP_LT,
	OP_GT,
	OP_LTE,
	OP_GTE,
	OP_EQ,
	OP_NEQ,
	OP_BOOL_MULT,
	OP_BOOL_PLUS,
	OP_BOOL_NOT,
	LINE_TERM,
	DELIMITER,
	LEFT_BRACE,
	RIGHT_BRACE,
	THIN_ARROW,
	QUOTATION_MARK,
	COLON,
	IDENTIFIER,
	LITERAL,
	STATIC_TYPE,
	NOP
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
char* concat_lexemes(LIST* tokens);

#endif
