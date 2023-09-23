#include <stdio.h>
#include <stdlib.h>
#include "include/ast_printer.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/list.h"
#include "include/hashmap.h"
#include "include/tac_generator.h"
#include "string.h"

int main(int argc, char **argv)
{
	// For debugging only
	const char *token_types[] = {"FUN", "BEGIN", "END", "DECLR", "RETURN", "COLON", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
								 "RIGHT_BRACKET", "COMMA", "LINE_TERM", "ASSIGN", "EQUALS", "PLUS", "MINUS", "MULT", "DIV",
								 "STATIC_TYPE", "IDENTIFIER", "INT_LITERAL"};

	const char *statement_types[] = {"ASSIGNMENT", "BLOCK", "PRINT", "EXPRESSION_STATEMENT"};

	LEXER *lexer = init_from_file("expr.msh");
	LIST* tokens = init_list(sizeof(TOKEN*));
	while (*lexer->current != '\0')
	{
		TOKEN *token = get_token(lexer);
		list_push(tokens, token);
	}
	for(int i = 0; i < tokens->size; i++) {
		TOKEN* token = tokens->elements[i];
		printf("line: %d lexeme: %s type: %s\n", token->line, token->lexeme, token_types[token->type]);
	}

	PARSER *parser = init_parser(tokens);
	AST_STMT* stmt = parser_parse(parser);
	free_lexer(lexer);
	if(parser->exited_with_error) {
		fprintf(stderr, "Compilation error\n");
		return 1;
	}
	print_ast_stmt(stmt);
	LIST* quads = init_list(sizeof(QUAD*));

	// H_MAP* registers = init_hash_map();
	// hashmap_put(H_MAP* registers, "rax", NULL);
	// hashmap_put(H_MAP* registers, "rbx", NULL);
	// hashmap_put(H_MAP* registers, "rcx", NULL);
	// hashmap_put(H_MAP* registers, "rdx", NULL);
	// hashmap_put(H_MAP* registers, "rdi", NULL);
	// hashmap_put(H_MAP* registers, "rsi", NULL);
	// hashmap_put(H_MAP* registers, "rdi", NULL);
	// hashmap_put(H_MAP* registers, "r8", NULL);
	// hashmap_put(H_MAP* registers, "r9", NULL);
	// hashmap_put(H_MAP* registers, "r10", NULL);
	// hashmap_put(H_MAP* registers, "r11", NULL);
	// hashmap_put(H_MAP* registers, "r12", NULL);
	// hashmap_put(H_MAP* registers, "r13", NULL);
	// hashmap_put(H_MAP* registers, "r14", NULL);
	// hashmap_put(H_MAP* registers, "r15", NULL);

	SYM_TABLE* table = init_symtable(NULL);
	quad_from_stmt(stmt, quads, table);

	// print quads
	// TODO: Move to printer
	for(int i = 0; i < quads->size; i++) {
		QUAD* q = list_get(quads, i);
		if(q->arg2 == NULL) {
			if(strcmp(q->op, "-") == 0) {
				printf("%s := %s %s\n", q->result, q->op, q->arg1);
			}
			if(strcmp(q->op, ":=") == 0) {
				printf("%s := %s\n", q->result, q->arg1);
			}

		} else if(strcmp(q->op, "declr") == 0) {
				printf("%s %s := %s\n", q->arg2, q->result, q->arg1);
		}

		else {
			printf("%s := %s %s %s\n", q->result, q->arg1, q->op, q->arg2);
		}
	}
	return 0;
}

