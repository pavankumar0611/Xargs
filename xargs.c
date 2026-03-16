#include "header.h"

#define MAX_ARGS 50
#define BUFFER_SIZE 250

void read_input(char *array[], int *count)
{
	char buffer[BUFFER_SIZE];
	int i = 1;

	/* to read input from stdin and convert into single argument */
	while (fgets(buffer, sizeof(buffer), stdin)) {
		char *token = strtok(buffer, " \n\t");

		while (token != NULL) {

			if (i >= MAX_ARGS - 1) {
				fprintf(stderr, "Too many arguments\n");
				break;
			}
			array[i++] = strdup(token);
			token = strtok(NULL, " \n\t");
		}
	}

	array[i] = NULL;
	*count = i;
}

void execute_command(char *cmd, char *array[])
{
	pid_t pid = fork();

	if (pid < 0) {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		execvp(cmd, array);
		perror("execvp failed");
		exit(EXIT_FAILURE);
	}
	else {
		int status;
		wait(&status);
	}
}

void free_memory(char *array[], int count)
{
	/* to remove the allocated memory done using strdup */
	for (int i = 1; i < count; i++) {
		free(array[i]);
	}
}

int main(int argc, char *argv[])
{
	char *array[MAX_ARGS];
	int count = 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <command>\n" "Example:\n"" cat file.txt | %s rm\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	/* argv[0] for execvp should be command name . so the array contains the a.out or rm in the array */
	array[0] = argv[1];

	read_input(array, &count);

	execute_command(argv[1], array);

	free_memory(array, count);

	return 0;
}
