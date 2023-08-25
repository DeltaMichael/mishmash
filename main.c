#include <stdio.h>
#include "include/lexer.h"
#include "string.h"

int main(int argc, char **argv)
{
	LEXER *lexer = init_from_file("program.msh");
	while (*lexer->current != '\0')
	{
		TOKEN *token = get_token(lexer);
		printf("%s\n", token->lexeme);
	}
	free_lexer(lexer);
}
