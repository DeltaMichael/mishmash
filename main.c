#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"
#include "include/list.h"
#include "string.h"

int main(int argc, char **argv)
{
	// For debugging only
	const char *token_types[] = {"FUN", "BEGIN", "END", "DECLR", "RETURN", "COLON", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
								 "RIGHT_BRACKET", "COMMA", "LINE_TERM", "ASSIGN", "EQUALS", "PLUS", "MINUS", "MULT", "DIV",
								 "STATIC_TYPE", "IDENTIFIER"};
	LEXER *lexer = init_from_file("program.msh");
	LIST* list = init_list(sizeof(TOKEN*));
	while (*lexer->current != '\0')
	{
		TOKEN *token = get_token(lexer);
		printf("lexeme: %s type: %s\n", token->lexeme, token_types[token->type]);
		list_push(list, token);
	}
	free_lexer(lexer);
}

