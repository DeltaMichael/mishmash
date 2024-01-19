#include "include/list.h"
#include <stdlib.h>
#include <stdio.h>


LIST* init_list(size_t element_size) {
	LIST* list = malloc(sizeof(LIST));
	list->element_size = element_size;
	list->size = 0;
	list->elements = 0;
	return list;
}

void list_push(LIST* list, void* element) {
	if(!list->elements) {
		list->elements = malloc(list->element_size);
	} else {
		void** new_elems = realloc(list->elements, (list->size + 1) * list->element_size);
		if(new_elems) {
			list->elements = new_elems;
		} else {
			printf("%s\n", "Could not allocate space for next element");
		}
	}
	list->elements[list->size] = element;
	list->size++;
}

void* list_pop(LIST* list) {
	void* element = list->elements[list->size - 1];
	void** new_elements = realloc(list->elements, (list->size - 1) * list->element_size);
	if(new_elements) {
		list->elements = new_elements;
	} else {
		printf("%s\n", "Could not de-allocate space when popping an element");
	}
	list->size--;
	return element;
}

void* list_get(LIST* list, int index) {
	if (index > list->size - 1 || index < 0) {
		return NULL;
	}
	void* element = list->elements[index];
	return element;
}

void free_list(LIST* list, void (*f)(void*)) {
	for(int i = 0; i < list->size; i++) {
		(*f)(list->elements[i]);
	}
	free(list->elements);
	free(list);
}

