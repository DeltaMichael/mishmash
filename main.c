#include <stdio.h>
#include <stdlib.h>
#include "include/asm_generator.h"
#include "include/ast_printer.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/list.h"
#include "include/string_builder.h"
#include "include/hashmap.h"
#include "include/tac_generator.h"
#include "string.h"

char* get_bare_path(char *source_file_path)
{
	char *bare_path = malloc(sizeof(char) * 256);
	int path_length = strlen(source_file_path);
	int ext_index = 0;
	for (int i = path_length; i >= 0; i--) {
		if (source_file_path[i] == '.') {
			ext_index = i;
			break;
		}
	}
	for (int i = 0; i < ext_index; i++) {
		bare_path[i] = source_file_path[i];
	}
	bare_path[ext_index] = 0;
	return bare_path;
}

void run_asm_and_linker(char *bare_path)
{
	char command[256];
	sprintf(command, "nasm -felf64 -g %s.asm -o %s.o", bare_path, bare_path);
	system(command);

	// TODO: Remove this call after supporting multiple source files with PIC
	// This is a compiler, this isn't 'Nam, there are rules here
	sprintf(command,
		"ld --dynamic-linker=/lib64/ld-linux-x86-64.so.2 %s.o /lib64/mashlib.so -o %s",
		bare_path, bare_path);
	system(command);
}

void clean_up(char *bare_path)
{
	char command[256];

	// TODO: Clean up just .asm files after supporting multiple source files with PIC
	// This is a compiler, this isn't 'Nam, there are rules here
	// sprintf(command, "rm %s.asm %s.o", bare_path, bare_path);
	sprintf(command, "rm %s.o", bare_path, bare_path);
	system(command);
}

void output_source_to_file(char *file_path, char *asm_source)
{
	int file_name_length = strlen(file_path);

	STRING_BUILDER *out_name_builder = init_sb();
	for (int i = file_name_length - 1; i >= 0; i--) {
		if (file_path[i] == '.') {
			file_name_length = i;
			break;
		}
	}
	for (int i = 0; i < file_name_length; i++) {
		sb_append_char(out_name_builder, file_path[i]);
	}
	sb_append(out_name_builder, ".asm");

	char *out_file_path = sb_build(out_name_builder);
	FILE *f = fopen(out_file_path, "w");
	fprintf(f, "%s", asm_source);
	free_sb(out_name_builder);
	fclose(f);
}

void usage()
{
	printf("Usage: mishmash <file_path>\n");
	printf("Options:\n");
	printf("-d : Debug output. Show lexer, AST and TAC output.\n");
}

int main(int argc, char **argv)
{
	if (argc == 1 || argc > 3) {
		usage();
		exit(1);
	}

	char *file_path;
	bool debug_output = false;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-d") == 0) {
				debug_output = true;
			} else {
				printf("Unknown flag: %s", argv[i]);
				usage();
				exit(1);
			}
		} else {
			file_path = argv[i];
		}
	}

	LEXER *lexer = init_from_file(file_path);
	LIST *tokens = init_list(sizeof(TOKEN *));
	while (*lexer->current != '\0') {
		TOKEN *token = get_token(lexer);
		if (token != NULL) {
			list_push(tokens, token);
		}
	}

	PARSER *parser = init_parser(tokens);
	AST_STMT *stmt = parser_parse(parser);
	if (parser->exited_with_error) {
		free_list(tokens, free_token);
		free_lexer(lexer);
		free_parser(parser);
		free_ast_stmt(stmt);
		fprintf(stderr, "Compilation error\n");
		return 1;
	}

	LIST *quads = init_list(sizeof(QUAD *));
	SYM_TABLE *table = init_symtable(NULL);
	quad_from_stmt(stmt, quads, table);

	ASM_GENERATOR *asm_gen = init_asm_generator(quads, table);
	ag_generate_code(asm_gen);
	char *out = ag_get_code(asm_gen);
	output_source_to_file(file_path, out);
	char *bare_path = get_bare_path(file_path);
	run_asm_and_linker(bare_path);
	clean_up(bare_path);
	free(bare_path);

	if (debug_output) {
		// debug output
		const char *token_types[] =
		    { "FUN", "BEGIN", "END", "DECLR", "RETURN", "COLON",
			"LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
			"RIGHT_BRACKET", "COMMA", "LINE_TERM", "PRINT_OP",
			"ASSIGN", "EQUALS", "LESS_THAN", "LESS_THAN_EQ", "GREATER_THAN", "GREATER_THAN_EQ", "PLUS",
			    "MINUS", "MULT", "DIV",
			"STATIC_TYPE", "IDENTIFIER", "INT_LITERAL"
		};

		const char *statement_types[] =
		    { "ASSIGNMENT", "BLOCK", "PRINT", "EXPRESSION_STATEMENT" };

		// debug output
		for (int i = 0; i < tokens->size; i++) {
			TOKEN *token = tokens->elements[i];
			printf("line: %d lexeme: %s type: %s\n", token->line,
			       token->lexeme, token_types[token->type]);
		}

		// debug output
		print_ast_stmt(stmt);

		// debug output
		for (int i = 0; i < quads->size; i++) {
			QUAD *q = list_get(quads, i);
			if (q->arg2 == NULL) {
				if (strcmp(q->op, "uminus") == 0) {
					printf("%s := %s %s\n", q->result,
					       q->op, q->arg1);
				}
				if (strcmp(q->op, "-") == 0) {
					printf("%s := %s %s\n", q->result,
					       q->op, q->arg1);
				}
				if (strcmp(q->op, ":=") == 0) {
					printf("%s := %s\n", q->result,
					       q->arg1);
				}
				if (strcmp(q->op, "print") == 0) {
					printf("print %s\n", q->arg1);
				}
			} else if (strcmp(q->op, "declr") == 0) {
				printf("%s %s := %s\n", q->arg2, q->result,
				       q->arg1);
			}

			else {
				printf("%s := %s %s %s\n", q->result, q->arg1,
				       q->op, q->arg2);
			}
		}

		// debug output
		printf("%s", out);
	}

	free_list(tokens, free_token);
	free_lexer(lexer);
	free_parser(parser);
	free_ast_stmt(stmt);
	free_asm_generator(asm_gen);

	return 0;
}

