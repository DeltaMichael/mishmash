
#ifndef TOKEN_MAPPER_H
#define TOKEN_MAPPER_H

#include "../common/list.h"
#include <stdio.h>

typedef struct {
	char lexeme[256];
	char type[256];
} TOKEN_MAPPING;

int is_string_whitespace(char *input);
TOKEN_MAPPING* get_token_mapping(char *input);
void write_token_types_to_h_file(LIST* mappings, FILE *out);
void write_token_mappings_to_c_file(LIST* mappings, FILE *out);
LIST* fetch_token_mappings(FILE *in);
void generate_token_defs();
#endif

