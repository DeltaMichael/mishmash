#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/list.h"

LIST *get_expected_values(char *test_file_name)
{
	LIST *output = init_list(sizeof(char *));

	char file_path[256];
	sprintf(file_path, "./tests/%s", test_file_name);
	FILE *file = fopen(file_path, "r");

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		int line_index = 0;
		int value_index = 0;
		// skip whitespace
		while (line[line_index] == ' ')
			line_index++;
		if (line[0] == '/' && line[1] == '/') {
			char *value = malloc(sizeof(char) * 256);
			// skip comment and whitespace
			line_index += 2;
			while (line[line_index] == ' ')
				line_index++;
			while (line[line_index] != 0) {
				value[value_index] = line[line_index];
				value_index++;
				line_index++;
			}
			value[value_index] = 0;
			list_push(output, value);
		}
	}
	fclose(file);
	return output;
}

LIST *get_compilation_output(char *test_file_name)
{
	LIST *output = init_list(sizeof(char *));

	char command[256];
	sprintf(command, "./mishmash ./tests/%s 2>&1", test_file_name);
	FILE *file = popen(command, "r");
	char *line = malloc(sizeof(char) * 256);
	while (fgets(line, 256, file)) {
		list_push(output, line);
		line = malloc(sizeof(char) * 256);
	}
	free(line);
	pclose(file);
	return output;
}

LIST *get_run_output(char *test_file_name)
{

	LIST *output = init_list(sizeof(char *));

	char bare_name[256];
	int name_length = strlen(test_file_name);
	for (int i = 0; i < name_length; i++) {
		if (test_file_name[i] == '.') {
			bare_name[i] = 0;
			break;
		}
		bare_name[i] = test_file_name[i];
	}

	char command[256];
	sprintf(command, "./tests/%s 2>&1", bare_name);
	FILE *file = popen(command, "r");

	char *line = malloc(sizeof(char) * 256);
	while (fgets(line, 256, file)) {
		list_push(output, line);
		line = malloc(sizeof(char) * 256);
	}
	free(line);
	pclose(file);
	return output;
}

void create_objects_and_link(char *test_file_name)
{
	char command[256];
	char bare_name[256];
	int name_length = strlen(test_file_name);
	for (int i = 0; i < name_length; i++) {
		if (test_file_name[i] == '.') {
			bare_name[i] = 0;
			break;
		}
		bare_name[i] = test_file_name[i];
	}
	sprintf(command, "as ./tests/%s.s -o ./tests/%s.o", bare_name,
		bare_name);
	system(command);

	sprintf(command,
		"ld --dynamic-linker=/lib64/ld-linux-x86-64.so.2 ./tests/%s.o /lib64/mashlib.so -o ./tests/%s",
		bare_name, bare_name);
	system(command);
}

void clean_up(char *test_file_name)
{
	char command[256];
	char bare_name[256];
	int name_length = strlen(test_file_name);
	for (int i = 0; i < name_length; i++) {
		if (test_file_name[i] == '.') {
			bare_name[i] = 0;
			break;
		}
		bare_name[i] = test_file_name[i];
	}
	sprintf(command, "rm ./tests/%s", bare_name);
	system("rm ./tests/*.s ./tests/*.o");
	system(command);
}

void print_expected_output(LIST *expected, LIST *actual)
{
	printf("EXPECTED OUTPUT:\n");
	for (int i = 0; i < expected->size; i++) {
		printf("%s", list_get(expected, i));
	}
	printf("ACTUAL OUTPUT:\n");
	for (int i = 0; i < actual->size; i++) {
		printf("%s", list_get(actual, i));
	}
}

bool assert_output(LIST *expected, LIST *actual)
{

	if (expected->size != actual->size) {
		printf("Output has %d lines. Expected %d lines.\n",
		       actual->size, expected->size);
		print_expected_output(expected, actual);
		return false;
	}

	bool passed = true;
	for (int i = 0; i < expected->size; i++) {
		char *expected_line = list_get(expected, i);
		char *actual_line = list_get(actual, i);
		if (strcmp(expected_line, actual_line) != 0) {
			passed = false;
			printf("Expected: %sActual: %s\n", expected_line,
			       actual_line);
		};
	}
	if (!passed) {
		print_expected_output(expected, actual);
	}
	return passed;
}

bool run_test(char *file_name)
{
	printf("----------%s----------\n", file_name);
	LIST *expected_values = get_expected_values(file_name);
	LIST *compilation_output = get_compilation_output(file_name);
	bool passed = true;
	if (compilation_output->size > 0) {
		passed = assert_output(expected_values, compilation_output);
	} else {
		create_objects_and_link(file_name);
		LIST *run_output = get_run_output(file_name);
		passed = assert_output(expected_values, run_output);
		clean_up(file_name);
		free_list(run_output, free);
	}
	if (passed) {
		// TODO collect failures in list and display at the end
		printf("SUCCESS\n");
	} else {
		printf("FAIL\n");
	}
	printf("----------%s----------\n\n", file_name);
	free_list(expected_values, free);
	free_list(compilation_output, free);
	return passed;
}

int main(int argc, char **argv)
{
	FILE *file = popen("ls -1 ./tests", "r");
	char line[256];
	int success_count = 0;
	int total_count = 0;
	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\n")] = 0;
		bool result = run_test(line);
		success_count += result;
		total_count++;
	}
	pclose(file);
	printf("Passed %d/%d\n", success_count, total_count);
	// run_test("sad_unexpected_token.msh");
	return 0;
}

