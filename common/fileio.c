#include "fileio.h"

char *read_file(char *path)
{
	FILE *f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);
	rewind(f);
	char *out = malloc(file_size + 1);
	fread(out, sizeof(char), file_size, f);
	out[file_size] = 0;
	fclose(f);
	return out;
}

void append_line(char *path, char *line) {
	FILE *f = fopen(path, "w+");
	fprintf(f, "%s\n", line);
	fclose(f);
}

