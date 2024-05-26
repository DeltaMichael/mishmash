#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdlib.h>

char *read_file(char *path);
void append_line(char *path, char *line);

#endif

