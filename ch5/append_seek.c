/**
 * Exercise 5.2
 *
 * Write a program that opens an existing file for writing with the O_APPEND
 * flag, and then seeks to the beginning of the file before writing some data.
 * Where does the data appear in the file? Why?
 *
 * The data will appear at the end of the file. Seeking to a different file
 * offset only affects reads when O_APPEND is set.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void showUsageAndExit(const char* program_name);

int main(int argc, char** argv) {
  if (argc != 3) {
    showUsageAndExit(argv[0]);
  }

  int fd = open(argv[1], O_WRONLY | O_APPEND);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    exit(EXIT_FAILURE);
  }

  if (write(fd, argv[2], strlen(argv[2])) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

void showUsageAndExit(const char* program_name) {
  fprintf(stderr, "Usage: %s pathname data\n", program_name);
  exit(EXIT_FAILURE);
}
