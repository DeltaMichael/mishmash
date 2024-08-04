#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/list.h"
#include "common/string_builder.h"
#include "common/hashmap.h"
#include "lang/lexer.h"
#include "lang/parser.h"
#include "lang/generated/token.h"

void usage()
{
	printf("Usage: mishmash <file_path>\n");
	printf("Options:\n");
	printf("-d : Debug output. Show lexer, AST and TAC output. Show assembly code if in compiler mode\n");
	printf("-i : Run in interpreter mode\n");
}

int main(int argc, char **argv)
{
	if (argc == 1 || argc > 4)
	{
		usage();
		exit(1);
	}

	char *file_path = argv[1];
	char *out;

	LEXER *lexer = lexer_init_from_file(file_path);
	LIST *tokens = lexer_process(lexer);
	PARSER *parser = parser_init(tokens);
	parser_parse(parser);

	// free token lexemes
	for (int i = 0; i < tokens->size; i++) {
		TOKEN* token = list_get(tokens, i);
		printf("LEXEME: %s TYPE: %d\n", token->lexeme, token->type);
		free(token->lexeme);
	}

	// free token list
	free_list(tokens);
	lexer_free(lexer);

	return 0;
}

