#include "parser.h"
#include "../common/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

PARSER* parser_init(LIST* tokens) {
	PARSER* parser = malloc(sizeof(PARSER));
	if (parser == NULL) {
		// TODO: Fix this when we add error handling
		printf("Could not init parser\n");
		exit(1);
	}
	parser->tokens = tokens;
	parser->index = 0;
	parser->prev = list_get(tokens, parser->index);
	parser->current = list_get(tokens, parser->index);
	parser->next = list_get(tokens, parser->index + 1);
	return parser;
}

TOKEN* parser_advance(PARSER* parser) {
	if (parser_is_at_end(parser)) {
		return NULL;
	}

	parser->prev = list_get(parser->tokens, parser->index);

	if (parser->tokens->size - parser->index == 2) {
		parser->current = list_get(parser->tokens, parser->index + 1);
		parser->next = list_get(parser->tokens, parser->index + 1);
		parser->index++;
		return parser->prev;
	}

	parser->current = list_get(parser->tokens, parser->index + 1);
	parser->next = list_get(parser->tokens, parser->index + 2);
	parser->index++;
	return parser->prev;
}

bool parser_match(PARSER* parser, TOKEN_TYPE type) {
	if (parser->current->type == type) {
		parser_advance(parser);
		return true;
	}
	return false;
}

bool parser_match_one_of(PARSER* parser, int count, ...) {
	va_list tokens;
	va_start(tokens, count);
	for (int i = 0; i < count; i++) {
		TOKEN_TYPE type = va_arg(tokens, TOKEN_TYPE);
		if (parser_match(parser, type)) {
			va_end(tokens);
			return true;
		}
	}
	va_end(tokens);
	return false;
}

bool parser_match_all(PARSER* parser, int count, ...) {
	va_list tokens;
	va_start(tokens, count);
	int index = parser->index;
	if(parser->tokens->size - (index + count) < 1) {
		return false;
	}
	for (int i = 0; i < count; i++) {
		TOKEN_TYPE type = va_arg(tokens, TOKEN_TYPE);
		TOKEN* token = list_get(parser->tokens, index);
		if (token->type != type) {
			return false;
		}
		index++;
	}
	parser->index = index - 1;
	parser_advance(parser);
	va_end(tokens);
	return true;
}

LIST* parser_get_prev(PARSER* parser, int count) {
	LIST* tokens = init_list(sizeof(TOKEN*));
	int token_count = count;
	if (parser->index + 1 < count) {
		token_count = parser->index + 1;
	}
	for (int i = token_count; i > 0; i--) {
		list_push(tokens, list_get(parser->tokens, parser->index - i));
	}
	return tokens;
}

void parser_eat(PARSER* parser, TOKEN_TYPE type) {
	if(!parser_match(parser, type)) {
		// TODO: Fix this when we introduce error handling
		printf("Expected LINE_TERM\n");
		exit(1);
	}
}

bool parser_is_at_end(PARSER* parser) {
	return parser->tokens->size - parser->index == 1;
}

void parser_parse(PARSER* parser) {
	while(!parser_is_at_end(parser)) {
		RULE* rule = line(parser);
		print_rule(rule);
		gen_code(rule);
		printf("\n");
	}
}

void free_parser(PARSER* parser) {

}

void print_rule(RULE* rule) {
	if(rule->token) {
		printf("%s:%d(", rule->token->lexeme, rule->op);
	} else {
		printf("%d(", rule->op);
	}
	if(rule->children != NULL) {
		for(int i = 0; i < rule->children->size; i++) {
			RULE* child = list_get(rule->children, i);
			print_rule(child);
		}
	}
	printf(")");
}

void gen_code(RULE* rule) {
	if(rule->token && rule->token->type == T_RULE_NAME) {
		printf("AST_EXPR* %s(PARSER* parser) {\n", rule->token->lexeme);
	}
	if(rule->children != NULL) {
		switch(rule->op) {
			case R_OR:
				for (int i = 0; i < rule->children->size; i++) {
					RULE* child = list_get(rule->children, i);
					if(i > 0) {
						printf("else ");
					}
					gen_code(child);
				}
				break;
			case R_AND: {
				LIST* splits = split_children(rule);
				for(int i = 0; i < splits->size; i++) {
					LIST* split = list_get(splits, i);
					RULE* child = list_get(split, 0);
					if(child->op == R_LEAF && child->token->type == T_TERMINAL) {
						if(i == 0) {
							printf("if(parser_match(parser, %d", (int)split->size);
						} else {
							printf(" && parser_match(parser, %d", (int)split->size);
						}
						for(int j = 0; j < split->size; j++) {
							RULE* child = list_get(split, j);
							printf(", %s", child->token->lexeme);
						}
						printf(")");
					}
					else if(child->op == R_LEAF && child->token->type == T_NONTERMINAL) {
						int j = 0;
						if(i == 0) {
							printf("if(%s(parser)", child->token->lexeme);
							j++;
						}
						while(j < split->size) {
							RULE* child = list_get(split, j);
							printf(" && %s(parser)", child->token->lexeme);
							j++;
						}
					} else {
						for(int j = 0; j < split->size; j++) {
							RULE* child = list_get(split, j);
							gen_code(child);
						}
					}
				}
				printf(") {\n");
				printf("\tTOKEN* op = parser->prev;\n"
				"\tAST_EXPR* expr = ast_expr_init(%s, op->type, op->lexeme);\n"
				"\tlist_push(expr->children, parser->prev_expr);\n"
				"\tparser->prev_expr = expr;\n", "PLACEHOLDER");
				printf("}\n");
			}
			case R_ZERO_OR_MORE:
				break;
			case R_ZERO_OR_ONE:
				break;
			case R_LEAF:
				break;
			case R_ONE_OR_MORE:
				break;
			default:
				break;
		}
	} else if (rule->op == R_LEAF) {
		if(rule->token->type == T_TERMINAL) {
			printf("if(parser_match(parser, %s)) {\n", rule->token->lexeme);
			printf("TOKEN* op = parser->prev;\n"
			"parser->prev_expr = ast_expr_init(%s, op->type, op->lexeme);\n", rule->token->lexeme);
			printf("}\n");
		}
		if(rule->token->type == T_NONTERMINAL) {
			printf("if(%s(parser)) {\n", rule->token->lexeme);
			printf("}\n");
		}
	}
}

LIST* split_children(RULE* rule) {
	LIST* out = init_list(sizeof(LIST*));
	if(rule->children == NULL || rule->children->size == 0) {
		return out;
	}
	LIST* section = init_list(sizeof(LIST*));
	RULE* current = list_get(rule->children, 0);
	list_push(section, current);
	RULE* prev = current;
	for(int i = 1; i < rule->children->size; i++) {
		current = list_get(rule->children, i);
		if(current->token != NULL && prev->token != NULL && current->token->type != prev->token->type) {
			list_push(out, section);
			section = init_list(sizeof(LIST*));
		} else if (current->op != prev->op) {
			list_push(out, section);
			section = init_list(sizeof(LIST*));
		}
		list_push(section, current);
		prev = current;
	}
	if(current->token == NULL || prev->token == NULL || current->token->type == prev->token->type) {
		list_push(out, section);
	} else if (current->op == prev->op) {
		list_push(out, section);
	}
	return out;
}

RULE* line(PARSER* parser) {
	if(parser_match(parser, T_RULE_NAME)) {
		TOKEN* name = parser->prev;
		RULE* rule = add(parser);
		rule->token = name;
		parser_eat(parser, T_LINE_TERM);
		return rule;
	}
	return NULL;
}

RULE* add(PARSER* parser) {
	RULE* first = basic(parser);
	LIST* children = init_list(sizeof(RULE));
	list_push(children, first);
	RULE* out = init_rule(R_OR, children, NULL);
	while(parser_match(parser, T_OR)) {
		RULE* rule = basic(parser);
		list_push(out->children, rule);
	}
	if(out->children->size > 1) {
		return out;
	}
	return first;
}

RULE* basic(PARSER* parser) {
	LIST* children = init_list(sizeof(RULE));
	while(parser_match(parser, T_TERMINAL) || parser_match(parser, T_NONTERMINAL) || parser_match(parser, T_LEFT_BRACE)) {
		if(parser->prev->type == T_LEFT_BRACE) {
			RULE* rule = add(parser);
			parser_eat(parser, T_RIGHT_BRACE);
			if(parser_match(parser, T_STAR)) {
				LIST* new_children = init_list(sizeof(RULE));
				list_push(new_children, rule);
				RULE* new_rule = init_rule(R_ZERO_OR_MORE, new_children, NULL);
				list_push(children, new_rule);
			} else {
				list_push(children, rule);
			}
		} else {
			RULE* rule = init_rule(R_LEAF, NULL, parser->prev);
			list_push(children, rule);
		}
	}
	if(children->size == 1 && ((RULE*)list_get(children, 0))->op == R_LEAF) {
		return list_get(children, 0);
	}
	return init_rule(R_AND, children, NULL);
}

RULE* init_rule(RULE_OP op, LIST* children, TOKEN* token) {
	RULE* rule = malloc(sizeof(RULE));
	rule->op = op;
	rule->children = children;
	rule->token = token;
	return rule;
}

