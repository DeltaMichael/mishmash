#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/list.h"

LIST* get_compilation_output(char* test_file_name) {
	LIST* output = init_list(sizeof(char*));

	char command[256];
	sprintf(command, "./mishmash ./tests/%s", test_file_name);
	FILE* file = popen(command, "r");

	char* line = malloc(sizeof(char) * 256);
	while (fgets(line, sizeof(line), file)) {
	  list_push(output, line);
	  line = malloc(sizeof(char) * 256);
	}
	pclose(file);
	return output;
}

LIST* get_run_output(char* test_file_name) {

	LIST* output = init_list(sizeof(char*));

	char bare_name[64];
	int name_length = strlen(test_file_name);
	for (int i = 0; i < name_length; i++) {
		if(test_file_name[i] == '.') {
			bare_name[i] = 0;
			break;
		}
		bare_name[i] = test_file_name[i];
	}

	char command[256];
	sprintf(command, "./tests/%s", bare_name);
	FILE* file = popen(command, "r");

	char* line = malloc(sizeof(char) * 256);
	while (fgets(line, sizeof(line), file)) {
	  list_push(output, line);
	  line = malloc(sizeof(char) * 256);
	}
	pclose(file);
	return output;
}

void create_objects_and_link(char* test_file_name) {
	char command[256];
	char bare_name[64];
	int name_length = strlen(test_file_name);
	for (int i = 0; i < name_length; i++) {
		if(test_file_name[i] == '.') {
			bare_name[i] = 0;
			break;
		}
		bare_name[i] = test_file_name[i];
	}
	sprintf(command, "as ./tests/%s.s -o ./tests/%s.o", bare_name, bare_name);
	system(command);

	sprintf(command, "ld --dynamic-linker=/lib64/ld-linux-x86-64.so.2 ./tests/%s.o /lib64/mashlib.so -o ./tests/%s", bare_name, bare_name);
	system(command);
}

void clean_up() {
	system("rm ./tests/*.s ./tests/*.o");
	system("rm ./tests/expr");
}

int main(int argc, char** argv) {
	LIST* output = get_compilation_output("expr.msh");
	for(int i = 0; i < output->size; i++) {
		printf("%s", list_get(output, i));
	}
	create_objects_and_link("expr.msh");
	LIST* run_output = get_run_output("expr.msh");
	for(int i = 0; i < run_output->size; i++) {
		printf("%s", list_get(run_output, i));
	}
	clean_up();
	// TODO: Implement comments and put expect output as a comment at top of test file
	// TODO: Free lists
	return 0;
}

