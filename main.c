#include <stdio.h>
#include "include/lexer.h"

int main(int argc, char** argv) {
	char* str = "my first string that)s gonna be parsed";
	LEXER* lexer = init(str);
	while(*lexer->current != '\0') {
		TOKEN* token = get_token(lexer);
		printf("%s\n", token->lexeme);
	}
	free_lexer(lexer);
}

