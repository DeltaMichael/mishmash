#ifndef RULE_H
#define RULE_H

#include "../parser.h"
#include "../ast.h"

AST_EXPR* line(PARSER* parser);
AST_EXPR* assignment(PARSER* parser);
AST_EXPR* expression(PARSER* parser);
AST_EXPR* comparison(PARSER* parser);
AST_EXPR* factor(PARSER* parser);
AST_EXPR* term(PARSER* parser);
AST_EXPR* unary(PARSER* parser);
AST_EXPR* var_declr(PARSER* parser);
AST_EXPR* func_call(PARSER* parser);
AST_EXPR* basic(PARSER* parser);

#endif

