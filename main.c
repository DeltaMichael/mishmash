#include <stdio.h>
#include <stdlib.h>
#include "include/ast_printer.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/list.h"
#include "include/hashmap.h"
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
	while(!parser_is_at_end(parser)) {
		AST_STMT* stmt = statement(parser);
		if(parser->has_error) {
			parser->has_error = 0;
			parser_sync(parser);
		}
		print_ast_stmt(stmt);
	}
	free_lexer(lexer);
}

