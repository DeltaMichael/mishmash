#ifndef TOKEN_H
#define TOKEN_H

#include "../../common/hashmap.h"
#include "../../common/list.h"

typedef enum {
	// blocks
	COND_START,
	COND_ALT,
	BLOCK_DECLR,
	BLOCK_START,
	BLOCK_END,
	// arithmetic binary operators
	OP_ASSIGN,
	OP_PLUS,
	OP_MINUS,
	OP_MULT,
	OP_DIV,

	// bool binary operators
	OP_LT,
	OP_GT,
	OP_LTE,
	OP_GTE,
	OP_EQ,
	OP_NEQ,
	OP_BOOL_MULT,
	OP_BOOL_PLUS,
	// bool unary operators
	OP_BOOL_NOT,
	// special chars
	LINE_TERM,
	DELIMITER,
	LEFT_BRACE,
	RIGHT_BRACE,
	THIN_ARROW,
	QUOTATION_MARK,
	COLON,
	// types
	STATIC_TYPE,
	// other
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

