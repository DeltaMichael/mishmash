#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "parser.h"

void print_ast_expr(AST_EXPR * root);
void print_ast_stmt(AST_STMT * root);

#endif
