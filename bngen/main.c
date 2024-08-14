#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../common/list.h"
#include "token_mapper.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
	LEXER* lexer = lexer_init_from_file("../grammar/simple.gram");
	LIST* tokens = lexer_process(lexer);
	for(int i = 0; i < tokens->size; i++) {
		TOKEN* token = list_get(tokens, i);
		printf("LINE: %d LEXEME: %s TYPE: %d\n", token->line, token->lexeme, token->type);
	}
	PARSER* parser = parser_init(tokens);
	parser_parse(parser);
	generate_token_defs();
	return 0;
}

