#ifndef HASHMAP_H
#define HASHMAP_H
#include <stdbool.h>
#define MAP_SIZE 100

typedef struct H_ENTRY {
	char* key;
	void* value;
	struct H_ENTRY* next;
} H_ENTRY;

typedef struct {
	H_ENTRY** entries;
} H_MAP;

int hash(char* key);
int hash_int(int key);
H_MAP* init_hashmap();
void free_hashmap(H_MAP* map);
void* hashmap_get(H_MAP* map, char* key);
void hashmap_put(H_MAP* map, char* key, void* value);
void print_entry(H_ENTRY* entry);
bool contains_key(H_MAP* map, char* key);
#endif

