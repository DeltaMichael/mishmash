#include <stdio.h>

int main(int argc, char **argv)
{
	const char* tokens_h[] = {
		"#ifndef TOKEN_H",
		"#define TOKEN_H",
		"#include \"../../common/hashmap.h\"",
		"#include \"../../common/list.h\"",
		"typedef enum {",
		"\tIDENTIFIER,",
		"\tLITERAL",
		"} TOKEN_TYPE;",
		"",
		"typedef struct {",
		"\tchar* lexeme;",
		"\tTOKEN_TYPE type;",
		"\tint line;",
		"} TOKEN;",
		"",
		"H_MAP *lexeme_token_map(char* token_file_path);",
		"void free_token(TOKEN *token);",
		"void free_token_list(LIST * list);",
		"TOKEN *get_token(H_MAP *token_map, char* lexeme, int line);",
		"",
		"#endif"
	};
	size_t tokens_h_size = sizeof(tokens_h) / sizeof(char*);
	FILE *f = fopen("out.h", "w+");
	for (int i = 0; i < tokens_h_size; i++) {
		fprintf(f, "%s\n", tokens_h[i]);
	}
	return 0;
}

