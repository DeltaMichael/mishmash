#ifndef LIST_H
#define LIST_H

#include <ctype.h>

typedef struct {
	size_t element_size;
	size_t size;
	void** elements;
} LIST;

LIST* init_list(size_t element_size);
void* list_pop(LIST* list);
void list_push(LIST* list, void* element);

#endif

