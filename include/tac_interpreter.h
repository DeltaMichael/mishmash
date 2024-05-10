#ifndef TAC_INTERPRETER_H
#define TAC_INTERPRETER_H

#include "tac_generator.h"
#include "list.h"
#include "hashmap.h"

void interpret(LIST *quads, H_MAP *var_table);

#endif

