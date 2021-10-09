/**
 * Exercise 5.3
 *
 * This exercise is designed to demonstrate why the atomicity guaranteed by
 * opening a file with the O_APPEND flag is necessary. Write a program that
 * takes up to three command-line arguments:
 *
 *     $ atomic_append filename num-bytes [x]
 *
 * This file should open the specified filename (creating it if necessary)
 * and append num-bytes bytes to the file by using write() to write a byte at a
 * time. By default, the program should open the file with the O_APPEND flag,
 * but if a third command-line argument (x) is supplied, then the O_APPEND flag
 * should be omitted, and instead the program should perform an lseek(fd, 0,
 * SEEK_END) call before each write(). Run two instances of this program at the
 * same time without the x argument to write 1 million bytes to the same file:
 *
 *     $ atomic_append f1 1000000 & atomic_append f1 1000000
 *
 * Repeat the same steps, writing to a different file, but this time specifying
 * the x argument:
 *
 *     $ atomic_append f2 1000000 x & atomic_append f2 1000000 x
 *
 * List the sizes of the files f1 and f2 using ls -l and explain the difference.
 *
 * When running without O_APPEND, there's a race condition. If one process
 * writes to the file after the other seeks to the end but before its next
 * write, the file descriptor is no longer at the end of the file, because the
 * file offset has advanced as a consequence of the write made by the other
 * process. Each process has its own entry in the open file table, so the offset
 * is not shared, even if both entries in the open file table point to the same
 * i-node.
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void showUsageAndExit(const char* program_name);

int main(int argc, char** argv) {
  if (argc < 3 || argc > 4 || strcmp(argv[1], "--help") == 0) {
    showUsageAndExit(argv[0]);
  }

  // Open file, adding append mode if there's a 3rd argument.
  int file_flags = O_WRONLY | O_CREAT;
  if (argc < 4) {
    file_flags |= O_APPEND;
  }
  int fd = open(argv[1], file_flags, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  int num_bytes = atoi(argv[2]);

  for (int i = 0; i < num_bytes; ++i) {
    write(fd, "x", 1);
    if (argc == 4) {
      lseek(fd, 0, SEEK_END);
    }
  }

  exit(EXIT_SUCCESS);
}

void showUsageAndExit(const char* program_name) {
  fprintf(stderr, "Usage: %s pathname num_bytes [x]\n", program_name);
  exit(EXIT_FAILURE);
}
