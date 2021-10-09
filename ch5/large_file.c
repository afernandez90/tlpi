/**
 * Exercise 5.1
 *
 * Modify the program in Listing 5-3 to use standard file I/O system calls
 * (open() and lseek()) and the off_t data type. Compile the program with the
 * _FILE_OFFSET_BITS macro set to 64, and test it to show that a large file can
 * be successfully created.
 *
 * On 64-bit systems, the macro is not required and large files are supported by
 * default.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void showUsageAndExit(const char* program_name);

int main(int argc, char** argv) {
  int fd;
  off_t off;

  if (argc != 3 || strcmp(argv[1], "--help") == 0) {
    showUsageAndExit(argv[0]);
  }

  fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  off = atoll(argv[2]);
  if (lseek(fd, off, SEEK_SET) == -1) {
    perror("lseek");
    exit(EXIT_FAILURE);
  }

  if (write(fd, "test", 4) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

void showUsageAndExit(const char* program_name) {
  fprintf(stderr, "Usage: %s pathname offset\n", program_name);
  exit(EXIT_FAILURE);
}
