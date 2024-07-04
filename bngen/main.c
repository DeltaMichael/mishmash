#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../common/list.h"

typedef struct {
	char lexeme[256];
	char type[256];
} TOKEN_MAPPING;

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

TOKEN_MAPPING* get_token_mapping(char *input)
{
	TOKEN_MAPPING* mapping = malloc(sizeof(TOKEN_MAPPING));
	char *local = input;
	int lexeme_index = 0;
	while (*local != '\0' && *local != '|') {
		if(!isspace(*local)) {
			mapping->lexeme[lexeme_index] = *local;
			lexeme_index++;
		}
		local++;
	}
	mapping->lexeme[lexeme_index] = '\0';

	if (*local != '|') {
		printf("Error parsing token definition %s\n", input);
		return NULL;
	}

	// trim whitespace on the left of the token type
	while (*local == '|' || isspace(*local)) {
		local++;
	}

	int type_index = 0;
	while (*local != '\0' && *local != '|') {
		if(!isspace(*local)) {
			mapping->type[type_index] = *local;
			type_index++;
		}
		local++;
	}
	mapping->type[type_index] = '\0';

	return mapping;
}

void write_token_types_to_h_file(LIST* mappings, FILE *out)
{
	for(int i = 0; i < mappings->size; i++) {
		TOKEN_MAPPING *mapping = list_get(mappings, i);
		fprintf(out, "\t%s,\n", mapping->type);
	}
}

void write_token_mappings_to_c_file(LIST* mappings, FILE *out)
{
	for(int i = 0; i < mappings->size; i++) {
		TOKEN_MAPPING *mapping = list_get(mappings, i);
		fprintf(out, "\tTOKEN_TYPE *type = malloc(sizeof(TOKEN_TYPE*));\n");
		fprintf(out, "\t*type = %s;\n", mapping->type);
		fprintf(out, "\thashmap_put(map, %s, type);\n",mapping->lexeme);
		fprintf(out, "\n");
	}
}

LIST* fetch_token_mappings(FILE *in) {
	char buf[256];
	LIST* out = init_list(sizeof(TOKEN_MAPPING*));

	while(fgets(buf, sizeof(buf), in) != NULL) {
		if (buf[0] == '#' || is_string_whitespace(buf)) {
			continue;
		}
		TOKEN_MAPPING *mapping = get_token_mapping(buf);
		list_push(out, mapping);
	}

	return out;
}

int main(int argc, char **argv)
{
	const char* token_h[] = {
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

	const char* token_c[] = {
		"#include \"token.h\"",
		"#include <stdlib.h>",
		"#include <stdio.h>",
		"#include <ctype.h>",
		"",
		"H_MAP *lexeme_token_map(char* token_file_path) {",
			"\tH_MAP *map = init_hashmap();",
			"\treturn map;",
		"}",
		"",
		"void free_token(TOKEN *token)",
		"{",
		"\tif (token == NULL)",
		"\t{",
		"\t\treturn;",
		"\t}",
		"\tif (token->lexeme != NULL)",
		"\t{",
		"\t\tfree(token->lexeme);",
		"\t\ttoken->lexeme = NULL;",
		"\t}",
		"\tfree(token);",
		"\ttoken = NULL;",
		"}",
		"",
		"void free_token_list(LIST *list)",
		"{",
		"\tfor(int i = 0; i < list->size; i++) {",
		"\t\tfree_token(list->elements[i]);",
		"\t}",
		"\tfree(list->elements);",
		"\tlist->elements = NULL;",
		"\tfree(list);",
		"\tlist = NULL;",
		"}",
		"",
		"TOKEN *get_token(H_MAP *token_map, char* lexeme, int line)",
		"{",
		"\tTOKEN *token = malloc(sizeof(TOKEN));",
		"\ttoken->lexeme = lexeme;",
		"\tif (contains_key(token_map, token->lexeme)) {",
		"\t\ttoken->type = *(TOKEN_TYPE*)hashmap_get(token_map, token->lexeme);",
		"\t} else if (isalpha(*token->lexeme)) {",
		"\t\ttoken->type = IDENTIFIER;",
		"\t} else if (isdigit(*token->lexeme)) {",
		"\t\ttoken->type = LITERAL;",
		"\t}",
		"\telse",
		"\t{",
		"\t\tprintf(\"Lexeme %s could not be classified as anything\",",
		"\t\t\t\ttoken->lexeme);",
		"\t\t// exit(1);",
		"\t}",
		"\treturn token;",
		"}"
	};

	size_t token_h_size = sizeof(token_h) / sizeof(char*);

	FILE *tokens_gram = fopen("../grammar/tokens.gram", "r");
	LIST *token_mappings = fetch_token_mappings(tokens_gram);
	fclose(tokens_gram);

	FILE *token_h_file = fopen("out.h", "w+");
	for (int i = 0; i < token_h_size; i++) {
		if (i == 5) {
			write_token_types_to_h_file(token_mappings, token_h_file);
		}
		fprintf(token_h_file, "%s\n", token_h[i]);
	}
	fclose(token_h_file);

	size_t token_c_size = sizeof(token_c) / sizeof(char*);

	FILE *token_c_file = fopen("out.c", "w+");
	for (int i = 0; i < token_c_size; i++) {
		if (i == 7) {
			write_token_mappings_to_c_file(token_mappings, token_c_file);
		}
		fprintf(token_c_file, "%s\n", token_c[i]);
	}
	return 0;
}

