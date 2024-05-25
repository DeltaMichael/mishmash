#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/list.h"
#include "common/string_builder.h"
#include "common/hashmap.h"
#include "lang/lexer.h"
#include "lang/generated/token.h"

// void output_source_to_file(char *file_path, char *asm_source)
// {
// 	int file_name_length = strlen(file_path);
//
// 	STRING_BUILDER *out_name_builder = init_sb();
// 	for (int i = file_name_length - 1; i >= 0; i--)
// 	{
// 		if (file_path[i] == '.')
// 		{
// 			file_name_length = i;
// 			break;
// 		}
// 	}
// 	for (int i = 0; i < file_name_length; i++)
// 	{
// 		sb_append_char(out_name_builder, file_path[i]);
// 	}
// 	sb_append(out_name_builder, ".asm");
//
// 	char *out_file_path = sb_build(out_name_builder);
// 	FILE *f = fopen(out_file_path, "w");
// 	fprintf(f, "%s", asm_source);
// 	free_sb(out_name_builder);
// 	fclose(f);
// }

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
	for (int i = 0; i < tokens->size; i++) {
		TOKEN* token = list_get(tokens, i);
		printf("LEXEME: %s TYPE: %d\n", token->lexeme, token->type);
	}

	return 0;
}

