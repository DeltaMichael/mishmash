#include <stdio.h>
#include <ctype.h>
#include <string.h>

int is_string_whitespace(char *input)
{
	while(*input != '\0') {
		if (!isspace(*input)) {
			return 0;
		}
		input++;
	}
	return 1;
}

char * get_token_type(char *input)
{
	char *local = input;
	while (*local != '\0' && *local != '|') {
		local++;
	}
	if (*local != '|') {
		printf("Error parsing token definition %s\n", input);
	}
	while (*local == '|' || isspace(*local)) {
		local++;
	}

	char *trim_pointer = local;

	// trim whitespace on the right
	while (!isspace(*trim_pointer)) {
		trim_pointer++;
	}
	*trim_pointer = '\0';

	// ignore STATIC_TYPE tokens for now
	if (strcmp(local, "STATIC_TYPE") == 0) {
		return NULL;
	}

	return local;
}

void write_tokens_from_file(FILE *in, FILE *out) {
			char buf[256];
			while(fgets(buf, sizeof(buf), in) != NULL) {
				if(buf[0] == '#' || is_string_whitespace(buf)) {
					continue;
				}
				char *token_type = get_token_type(buf);
				if (token_type == NULL) {
					continue;
				}
				fprintf(out, "\t%s,\n", token_type);
			}
}

int main(int argc, char **argv)
{
	const char* tokens_h[] = {
		"#ifndef TOKEN_H",
		"#define TOKEN_H",
		"#include \"../../common/hashmap.h\"",
		"#include \"../../common/list.h\"",
		"typedef enum {",
		"\tIDENTIFIER,",
		"\tLITERAL,",
		"\tSTATIC_TYPE",
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
		if (i == 5) {
			char buf[256];
			FILE *tokens_gram = fopen("../grammar/tokens.gram", "r");
			while(fgets(buf, sizeof(buf), tokens_gram) != NULL) {
				if(buf[0] == '#' || is_string_whitespace(buf)) {
					continue;
				}
				char *token_type = get_token_type(buf);
				if (token_type == NULL) {
					continue;
				}
				fprintf(f, "\t%s,\n", token_type);
			}
			fclose(tokens_gram);
		}
		fprintf(f, "%s\n", tokens_h[i]);
	}
	return 0;
}

