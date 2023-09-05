#ifndef HASHMAP_H
#define HASHMAP_H

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
H_MAP* init_hashmap();
void* hashmap_get(H_MAP* map, char* key);
void hashmap_put(H_MAP* map, char* key, void* value);

#endif

