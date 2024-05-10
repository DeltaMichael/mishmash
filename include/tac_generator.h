#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include "list.h"
#include "parser.h"
#include "hashmap.h"

typedef struct SYM_TABLE
{
	int index;
	int depth;
	int offset;
	H_MAP *variables;
	struct SYM_TABLE *enclosing;
} SYM_TABLE;

typedef enum
{
	Q_SKIP,
	Q_DECLR,
	Q_PRINT,
	Q_LABEL,
	Q_GOTO,
	Q_IFFALSE,
	Q_ASSIGN,
	Q_ADD,
	Q_SUB,
	Q_DIV,
	Q_MUL,
	Q_UMINUS,
	Q_EQ,
	Q_LT,
	Q_LTE,
	Q_GT,
	Q_GTE
} QUAD_OP;

typedef struct
{
	QUAD_OP operation;
	char *op;
	char *arg1;
	char *arg2;
	char *result;
} QUAD;

typedef enum
{
	REGISTER,
	STACK,
	HEAP
} VAR_LOCATION;

typedef struct
{
	char *reg_name;
	char *type;
	int offset;
	VAR_LOCATION location;
	int first_index;
	int last_index;
} VAR_DATA;

QUAD *init_quad(QUAD_OP operation, char *op, char *arg1, char *arg2, char *result);
void free_quad(QUAD *quad);
QUAD *quad_from_expr(AST_EXPR *expr, LIST *quads, SYM_TABLE *sym_table);
QUAD *quad_from_stmt(AST_STMT *stmt, LIST *quads, SYM_TABLE *sym_table);

SYM_TABLE *init_symtable(SYM_TABLE *enclosing);
void free_symtable(SYM_TABLE *table);
void symtable_init_var(SYM_TABLE *table, char *name, VAR_LOCATION location,
					   char *type);
char *get_label(char type, size_t index);
char *symtable_get_temp(SYM_TABLE *table);
VAR_DATA *init_var_data(VAR_LOCATION location, char *type, int offset);

#endif

