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
	return hash_int(sum);
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

int hash_int(int key) {
	// square the key
	key *= key;

	// get the number of digits
	int digits = (int)ceil(log10(key));

	// divide into even halves +-1
	int upper_digs = digits - digits / 2 - digits % 2;
	int lower_digs = digits / 2;
	// get the factors
	int upper = (int)pow(10, upper_digs);
	int lower = (int)pow(10, lower_digs);

	// get the upper middle digit
	int upper_digit = (key / upper) % 10;
	// get the lower middle digit
	int lower_digit = (key % upper)/(lower/10);
	return upper_digit * 10 + lower_digit;
}

void print_entry(H_ENTRY* entry) {
	while(entry != NULL) {
		printf("{ key: %s value: %d }", entry->key, *(int*)entry->value);
		entry = entry->next;
	}
	printf("\n");
}

