#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const size_t BUFFER_SIZE = 512;

void usage_exit(const char* program_name) {
	fprintf(stderr, "Usage: %s file [-a (append mode)]\n", program_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	// Verify number of arguments or print help.
	if (argc > 3 || argc == 2 && strcmp(argv[1], "--help") == 0) {
		usage_exit(argv[0]);
	}

	// Check whether user has requested append mode.
	int file_flags = O_TRUNC;
	int get_opt_res;
        while ((get_opt_res = getopt(argc, argv, "a")) != -1) {
		// Invalid option.
		if (get_opt_res != 'a') {
			usage_exit(argv[0]);
		}
		file_flags = O_APPEND;
	}

	// Check for unexpected arguments.
    if (optind != argc - 1) {
        usage_exit(argv[0]);
    }

	// Open file.
	char* path = argv[optind];
	int fd = open(path, 
		      file_flags | O_CREAT | O_WRONLY, 
		      S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | 
		      S_IROTH | S_IWOTH);
	if (fd == -1) {
		fprintf(stderr, "Can't open %s for writing: ", path);
		perror("");
		exit(EXIT_FAILURE);
	}

	// Read until EOF.
	char buffer[BUFFER_SIZE];
	ssize_t read_result;
	while (read_result = read(STDIN_FILENO, buffer, BUFFER_SIZE)) {
		if (read_result == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		write(STDOUT_FILENO, buffer, read_result);
		write(fd, buffer, read_result);
	}
	
	exit(EXIT_SUCCESS);
}
