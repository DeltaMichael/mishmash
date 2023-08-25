#include <stdio.h>
#include "include/lexer.h"
#include "string.h"

int main(int argc, char **argv)
{

	const char *token_types[] = {"FUN", "BEGIN", "END", "DECLR", "RETURN", "COLON", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
								 "RIGHT_BRACKET", "COMMA", "LINE_TERM", "ASSIGN", "EQUALS", "PLUS", "MINUS", "MULT", "DIV",
								 "STATIC_TYPE", "IDENTIFIER"};
	LEXER *lexer = init_from_file("program.msh");
	while (*lexer->current != '\0')
	{
		TOKEN *token = get_token(lexer);
		printf("lexeme: %s type: %s\n", token->lexeme, token_types[token->type]);
	}
	free_lexer(lexer);
}
