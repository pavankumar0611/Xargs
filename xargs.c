#include "header.h"

#define MAX_ARGS 50
#define BUFFER_SIZE 250

void read_input(char *array[], int *count)
{
	char buffer[BUFFER_SIZE];
	int i = 0;

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

void execute_in_chunks(char *array[], int total, int n, char *cmd_argv[], int cmd_argc)
{
	int i = 0;
	while (i < total) {

		char *exec_args[MAX_ARGS];
		int j = 0;

		// Add command + its arguments
		for (int k = 0; k < cmd_argc; k++) {
			exec_args[j++] = cmd_argv[k];
		}

		int count = 0;

		// Adding  n number of  input arguments
		while (i < total && count < n) {
			exec_args[j++] = array[i++];
			count++;
		}

		exec_args[j] = NULL;

		pid_t pid = fork();

		if (pid < 0) {
			perror("fork failed");
			exit(1);
		}

		if (pid == 0) {
			execvp(exec_args[0], exec_args);
			perror("execvp failed");
			exit(1);
		} else {
			wait(NULL);
		}
	}
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
	int count = 0, opt , max_arguments = 0;

	if (argc <  2) {
		fprintf(stderr, "Usage: %s <command>\n" "Example:\n"" cat file.txt | %s rm\n", argv[0], argv[0]);
		exit(EXIT_FAILURE);
	}

	while((opt = getopt(argc, argv , "+n:")) != -1) {
		switch ( opt) {
			case 'n':
				max_arguments = atoi(optarg);  // function to convert string into integer
				break;
		}
	}

	char *cmd_argv[MAX_ARGS];
	int cmd_argc = 0;

	if (optind < argc) {
		for (int i = optind; i < argc; i++) {
			cmd_argv[cmd_argc++] = argv[i];
		}
	} else {
		cmd_argv[cmd_argc++] = "echo";  // default
	}
	cmd_argv[cmd_argc] = NULL;

	read_input(array, &count);

	if (!max_arguments)
		execute_command(argv[1], array);
	else
		execute_in_chunks(array, count, max_arguments, cmd_argv, cmd_argc);

	free_memory(array, count);

	return 0;
}
