#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <ctype.h>
#include <stddef.h>

typedef struct {
	size_t size;
	char* characters;
} STRING_BUILDER;

STRING_BUILDER* init_sb();
void free_sb(STRING_BUILDER *sb);
char sb_pop(STRING_BUILDER* builder);
char sb_get(STRING_BUILDER* builder, int index);
void sb_append_char(STRING_BUILDER* builder, char c);
void sb_append_int(STRING_BUILDER* builder, int number);
void sb_append(STRING_BUILDER* builder, char* c);
char* sb_build(STRING_BUILDER* builder);

#endif

