#include "include/string_builder.h"
#include <stdlib.h>
#include <stdio.h>

STRING_BUILDER* init_sb() {
	STRING_BUILDER* sb = malloc(sizeof(STRING_BUILDER));
	sb->size = 0;
	sb->characters = NULL;
	return sb;
}

char sb_pop(STRING_BUILDER* sb) {
	char c = sb->characters[sb->size - 1];
	char* new_characters = realloc(sb->characters, (sb->size - 1) * sizeof(char));
	if(new_characters) {
		sb->characters = new_characters;
	} else {
		printf("%s\n", "Could not de-allocate space when popping an element");
	}
	sb->size--;
	return c;

}

char sb_get(STRING_BUILDER* sb, int index) {
	if (index > sb->size - 1 || index < 0) {
		return 0;
	}
	char c = sb->characters[index];
	return c;
}

void sb_append_char(STRING_BUILDER* sb, char c) {
	if(!sb->characters) {
		sb->characters = malloc(sizeof(char));
	} else {
		char* newchars = realloc(sb->characters, (sb->size + 1) * sizeof(char));
		if(newchars) {
			sb->characters = newchars;
		} else {
			printf("%s\n", "Could not allocate space for next character");
		}
	}
	sb->characters[sb->size] = c;
	sb->size++;
}

// TODO: optimize this
void sb_append(STRING_BUILDER* sb, char* c) {
	while(*c != '\0') {
		sb_append_char(sb, *c);
		c += 1;
	}
}

char* sb_build(STRING_BUILDER* sb) {
	sb_append_char(sb, '\0');
	return sb->characters;
}

