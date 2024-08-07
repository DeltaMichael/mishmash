#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../common/list.h"

typedef struct {
	char lexeme[256];
	char type[256];
} TOKEN_MAPPING;

typedef struct {
	char name[256];
	LIST* productions;
} RULE;

typedef enum {
	ALL,
	MATCH_TERM,
	ONE_OR_MORE,
	ZERO_OR_MORE,
	ZERO_OR_ONE,
} PROD_TYPE;

typedef struct {
	LIST* terms;
	PROD_TYPE type;
} PRODUCTION;

typedef struct {
	char* id;
	int is_terminator;
} TERM;

int letter = 'a';

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
		if (strcmp("STATIC_TYPE", mapping->type) != 0) {
			fprintf(out, "\t%s,\n", mapping->type);
		}
	}
}

void write_token_mappings_to_c_file(LIST* mappings, FILE *out)
{
	for(int i = 0; i < mappings->size; i++) {
		TOKEN_MAPPING *mapping = list_get(mappings, i);
		fprintf(out, "\ttype = malloc(sizeof(TOKEN_TYPE*));\n");
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

void generate_token_defs()
{
	const char* token_h[] = {
		"#ifndef TOKEN_H",
		"#define TOKEN_H",
		"#include \"../../common/hashmap.h\"",
		"#include \"../../common/list.h\"",
		"typedef enum {",
		"\tIDENTIFIER,",
		"\tLITERAL,",
		"\tSTATIC_TYPE,",
		"\tNOP",
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
		"char* concat_lexemes(LIST* tokens);",
		"",
		"#endif"
	};

	const char* token_c[] = {
		"#include \"token.h\"",
		"#include <stdlib.h>",
		"#include <stdio.h>",
		"#include <string.h>",
		"#include <ctype.h>",
		"",
		"H_MAP *lexeme_token_map(char* token_file_path) {",
			"\tH_MAP *map = init_hashmap();",
			"\tTOKEN_TYPE* type;",
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
		"}",
		"char* concat_lexemes(LIST* tokens) {",
		"\tint size = 0;",
		"\tfor (int i = 0; i < tokens->size; i++) {",
		"\t\tsize += strlen(((TOKEN*)(list_get(tokens, i)))->lexeme);",
		"\t}",
		"\tchar *op = malloc(size * sizeof(char) + 1);",
		"",
		"\tstrcpy(op, ((TOKEN*)(list_get(tokens, 0)))->lexeme);",
		"\tfor (int i = 1; i < tokens->size; i++) {",
		"\t\tstrcat(op, ((TOKEN*)(list_get(tokens, i)))->lexeme);",
		"\t}",
		"\treturn op;",
		"}"
	};

	size_t token_h_size = sizeof(token_h) / sizeof(char*);

	FILE *tokens_gram = fopen("../grammar/tokens.gram", "r");
	LIST *token_mappings = fetch_token_mappings(tokens_gram);
	fclose(tokens_gram);

	FILE *token_h_file = fopen("../lang/generated/token.h", "w+");
	for (int i = 0; i < token_h_size; i++) {
		if (i == 5) {
			write_token_types_to_h_file(token_mappings, token_h_file);
		}
		fprintf(token_h_file, "%s\n", token_h[i]);
	}
	fclose(token_h_file);

	size_t token_c_size = sizeof(token_c) / sizeof(char*);

	FILE *token_c_file = fopen("../lang/generated/token.c", "w+");
	for (int i = 0; i < token_c_size; i++) {
		if (i == 9) {
			write_token_mappings_to_c_file(token_mappings, token_c_file);
		}
		fprintf(token_c_file, "%s\n", token_c[i]);
	}
	fclose(token_c_file);
}

void print_rule(RULE* rule) {
	printf("%s: ", rule->name);
	for (int i = 0; i < rule->productions->size; i++) {
		PRODUCTION* production = list_get(rule->productions, i);
		for(int j = 0; j < production->terms->size; j++) {
			TERM* term = list_get(production->terms, j);
			printf("%s ", term->id);
		}
		printf("%d ", production->type);
	}
	printf("\n");
}

int is_special_char(char c) {
	switch(c) {
		case '(':
		case ')':
		case '*':
		case '?':
		case '+':
		case '$':
		case '|': {
			return 1;
		}
		default:
			return 0;
	}
}

LIST* split(char* in, int index) {
	LIST* out = init_list(sizeof(TERM));
	while(isspace(in[index])) {
		index++;
	}
	while(in[index] != '\0') {
		if(is_special_char(in[index])) {
			char* token = malloc(2 * sizeof(char));
			token[0] = in[index];
			token[1] = '\0';
			index++;
			list_push(out, token);
		}
		else if(isspace(in[index])) {
			index++;
		} else {
			char* token = malloc(64 * sizeof(char));
			int token_index = 0;
			while(!isspace(in[index]) && !is_special_char(in[index])) {
					token[token_index] = in[index];
					token_index++;
					index++;
			}
			token[token_index] = '\0';
			list_push(out, token);
		}
	}
	return out;
}

PRODUCTION* init_production(LIST* terms, char op) {
	PRODUCTION* production = malloc(sizeof(PRODUCTION));
	switch (op) {
		case '$': production->type = MATCH_TERM; break;
		case '?': production->type = ZERO_OR_ONE; break;
		case '+': production->type = ONE_OR_MORE; break;
		case '*': production->type = ZERO_OR_MORE; break;
		default: production->type = ALL; break;
	}
	production->terms = terms;
	return production;
}

void buffer_from_braces(int* i, char* new_buffer, LIST* tokens) {
	int j = *i + 1;
	int count = 1;
	while (count > 0) {
		char *local_token = list_get(tokens, j);
		if(local_token[0] == '(') {
			count++;
		}
		if(local_token[0] == ')') {
			count--;
		}
		j++;
	}
	int start = *i + 1;
	int end = j - 1;
	new_buffer[0] = letter;
	letter++;
	new_buffer[1] = ':';
	new_buffer[2] = ' ';
	int new_buffer_index = 3;
	for(int i = start; i <= end; i++) {
		char* token = list_get(tokens, i);
		int j = 0;
		while(token[j] != '\0') {
			new_buffer[new_buffer_index] = token[j];
			j++;
			new_buffer_index++;
		}
		new_buffer[new_buffer_index] = ' ';
		new_buffer_index++;
	}
	new_buffer[new_buffer_index] = '\0';
	*i = end;
}

RULE* create_rule(char* buf, LIST* rules) {
	RULE* rule = malloc(sizeof(RULE));
	rule->productions = init_list(sizeof(PRODUCTION));

	int i = 0;
	for (i = 0; i < 256; i++) {
		if(buf[i] == ':') {
			rule->name[i] = '\0';
			break;
		}
		rule->name[i] = buf[i];
	}

	LIST* tokens = split(buf, i + 1);
	LIST* terms = init_list(sizeof(TERM));
	for(int i = 0; i < tokens->size; i++) {
		char* token = list_get(tokens, i);
		// printf("TOKEN: %s\n", token);
		if(is_special_char(token[0])) {
			if (token[0] == '(') {
				if(terms->size > 0) {
					PRODUCTION* production = init_production(terms, ' ');
					list_push(rule->productions, production);
					terms = init_list(sizeof(TERM));
				}
				char* new_buf = malloc(256 * sizeof(char));
				buffer_from_braces(&i, new_buf, tokens);
				RULE* rule = create_rule(new_buf, rules);
				TERM* term = malloc(sizeof(TERM));
				term->id = rule->name;
				term->is_terminator = 0;
				list_push(terms, term);
			} else {
				if(!is_string_whitespace(token) && terms->size > 0) {
					PRODUCTION* production = init_production(terms, token[0]);
					list_push(rule->productions, production);
					terms = init_list(sizeof(TERM));
				}
			}
		} else {
			TERM* term = malloc(sizeof(TERM));
			term->id = token;
			term->is_terminator = isupper(token[0]);
			list_push(terms, term);
		}
	}
	if(terms->size > 0) {
		PRODUCTION* production = init_production(terms, ' ');
		list_push(rule->productions, production);
	}
	list_push(rules, rule);
	return rule;
}

int main(int argc, char **argv)
{
	FILE *simple_gram = fopen("../grammar/simple.gram", "r");
	char buf[256];
	LIST* rules = init_list(sizeof(RULE));
	while(fgets(buf, sizeof(buf), simple_gram) != NULL) {
		if (buf[0] == '#' || is_string_whitespace(buf)) {
			continue; }
		create_rule(buf, rules);
	}
	for(int i = 0; i < rules->size; i++) {
		RULE* rule = list_get(rules, i);
		print_rule(rule);
	}
	fclose(simple_gram);
}

