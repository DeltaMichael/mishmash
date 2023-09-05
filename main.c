#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/list.h"
#include "include/hashmap.h"
#include "string.h"
void print_ast(AST_EXPR* root) {
	if(!root->children) {
		printf("%s ", root->op->lexeme);
		return;
	} else {
		printf("(");
		for(int i = 0; i < root->children->size; i++) {
			print_ast(root->children->elements[i]);
		}
	}
	printf("%s) ", root->op->lexeme);
}

int main(int argc, char **argv)
{
	// For debugging only
	const char *token_types[] = {"FUN", "BEGIN", "END", "DECLR", "RETURN", "COLON", "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACKET",
								 "RIGHT_BRACKET", "COMMA", "LINE_TERM", "ASSIGN", "EQUALS", "PLUS", "MINUS", "MULT", "DIV",
								 "STATIC_TYPE", "IDENTIFIER", "INT_LITERAL"};
	LEXER *lexer = init_from_file("expr.msh");
	LIST* tokens = init_list(sizeof(TOKEN*));
	while (*lexer->current != '\0')
	{
		TOKEN *token = get_token(lexer);
		printf("lexeme: %s type: %s\n", token->lexeme, token_types[token->type]);
		list_push(tokens, token);
	}
	for(int i = 0; i < tokens->size; i++) {
		printf("%s\n", ((TOKEN*)tokens->elements[i])->lexeme);
	}
	PARSER *parser = init_parser(tokens);
	AST_EXPR* expr = expression(parser);
	print_ast(expr);
	free_lexer(lexer);

	// hashmap working at first glance
	int* val = malloc(sizeof(int));
	*val = 24;
	H_MAP* map = init_hashmap();
	hashmap_put(map, "something", val);
	int* outval = hashmap_get(map, "something");
	printf("%d", *outval);

}

