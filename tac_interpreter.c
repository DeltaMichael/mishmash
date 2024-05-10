#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/tac_interpreter.h"

#define fetch_with_type(type, key, map) ({type retval; if (contains_key(map, key)) { retval = *(type *)hashmap_get(map, key);} else { retval = atoi(key); } retval;})

#define fetch_or_create_temp(type, key, map) ({type retval; if (contains_key(map, key)) { retval = hashmap_get(map, key);} else { retval = calloc(1, sizeof(type)); hashmap_put(map, key, retval); } retval;})

void interpret(LIST *quads, H_MAP *var_table)
{

	H_MAP *variables = init_hashmap();
	H_MAP *labels = init_hashmap();
	for (int i = 0; i < quads->size; i++)
	{
		QUAD *q = list_get(quads, i);
		if (q->operation == Q_LABEL)
		{
			int *val = calloc(1, sizeof(int));
			*val = i;
			hashmap_put(labels, q->arg1, val);
		}
	}
	for (int i = 0; i < quads->size; i++)
	{
		QUAD *q = list_get(quads, i);
		switch(q->operation) {
				case Q_DECLR:
					{
						void *space;
						if (strcmp(q->arg2, "int") == 0) {
							space = calloc(1, sizeof(int));
							hashmap_put(variables, q->arg1, space);
						} else if (strcmp(q->arg2, "bool") == 0) {
							space = calloc(1, sizeof(int));
						}
						hashmap_put(variables, q->arg1, space);
						break;
					}
				case Q_PRINT:
					{
						int val = fetch_with_type(int, q->arg1, variables);
						printf("%d\n", val);

						break;
					}
				case Q_GOTO:
					i = fetch_with_type(int, q->arg1, labels);
					break;
				case Q_IFFALSE:
					{
						int *var = hashmap_get(variables, q->arg1);
						if (*var == 0) { i = fetch_with_type(int, q->result, labels); };
						break;
					}
				case Q_ASSIGN:
					{
						int *var = hashmap_get(variables, q->result);
						*var = fetch_with_type(int, q->arg1, variables);
						break;
					}
				case Q_ADD:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first + second;
						break;
					}
				case Q_SUB:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first - second;
						break;
					}
				case Q_DIV:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first / second;
						break;
					}
				case Q_MUL:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first * second;
						break;
					}
				case Q_UMINUS:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						*var = first * -1;
						break;
					}
				case Q_EQ:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first == second;
						break;
					}
				case Q_LT:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first < second;
						break;
					}
				case Q_LTE:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first <= second;
						break;
					}
				case Q_GT:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first > second;
						break;
					}
				case Q_GTE:
					{
						int *var = fetch_or_create_temp(int*, q->result, variables);
						int first = fetch_with_type(int, q->arg1, variables);
						int second = fetch_with_type(int, q->arg2, variables);
						*var = first >= second;
						break;
					}
				case Q_LABEL:
					break;
				case Q_SKIP:
					break;
			}
	}
}

