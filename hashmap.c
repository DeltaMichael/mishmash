#include "include/hashmap.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(char* key) {
	// sum the string characters
	int sum = 0;
	while(*key != '\0') {
		sum += *key;
		key += sizeof(char);
	}
	// square the sum
	sum *= sum;

	// get the number of digits in the sum
	int digits = (int)ceil(log10(sum));

	// get the front half divisor
	int front = (int)pow(10, digits/2);

	// get the back half divisor
	int back = (int)pow(10, digits - digits/2);

	// get middle two digits
	int first = (sum/front) % 10;
	int second = (sum%front) / (back / 10);

	// return them as a number
	// this will also work if one of them is 0
	return first * 10 + second;
}
H_MAP* init_hashmap() {
	H_MAP* map = malloc(sizeof(H_MAP));
	H_ENTRY** entries = malloc(MAP_SIZE * (sizeof(H_ENTRY*)));
	for(int i = 0; i < MAP_SIZE; i++) {
		entries[i] = NULL;
	}
	map->entries = entries;
	return map;
}
void* hashmap_get(H_MAP* map, char* key) {
	int index = hash(key);
	H_ENTRY* entry = map->entries[index];

	if(entry == NULL) {
		return NULL;
	}

	while(strcmp(key, entry->key) != 0) {
		entry = entry->next;
	}
	return entry->value;
}

// this will produce a lot of garbage on re-writing
// the same key
void hashmap_put(H_MAP* map, char* key, void* value) {
	int index = hash(key);
	H_ENTRY* entry = map->entries[index];
	H_ENTRY* new_entry = malloc(sizeof(H_ENTRY));
	new_entry->key = key;
	new_entry->value = value;
	new_entry->next = NULL;
	if(entry != NULL) {
		new_entry->next = entry;
	}
	map->entries[index] = new_entry;
}

