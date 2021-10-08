/**
 * Exercise 4.1
 *
 * The `tee` command reads its standard input until end-of-file, writing a copy
 * of the input to standard output and to the file named in its command-line
 * argument. (We show an example of the use of this command when we discuss
 * FIFOs in Section 44.7.) Implement `tee` using I/O system calls. By default,
 * `tee` overwrites any existing file with the given name. Implement the `-a`
 * command-line option (`tee -a file`), which causes `tee` to append text to the
 * end of a file if it already exists. (Refer to Appendix B for a description of
 * the `getopt()` function, which can be used to parse command-line options.)
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

blksize_t blockSizeForIO(int fd);
void showUsageAndExit(const char* program_name);

int main(int argc, char** argv) {
  if (argc == 2 && strcmp(argv[1], "--help") == 0 || argc == 1 || argc > 3) {
    showUsageAndExit(argv[0]);
  }

  // Check whether user has requested append mode.
  int file_flags = O_TRUNC;
  int get_opt_res;
  while ((get_opt_res = getopt(argc, argv, "a")) != -1) {
    // Invalid option.
    if (get_opt_res != 'a') {
      showUsageAndExit(argv[0]);
    }
    file_flags = O_APPEND;
  }

  // Only a file name should be left as the last argument after parsing options.
  // If optind doesn't point to the last argument, there's more than one
  // non-option argument.
  if (optind != argc - 1) {
    showUsageAndExit(argv[0]);
  }

  // Open output file.
  char* path = argv[optind];
  int fd = open(path, file_flags | O_CREAT | O_WRONLY,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd == -1) {
    fprintf(stderr, "Can't open %s for writing: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Allocate write buffer.
  blksize_t buffer_size = blockSizeForIO(fd);
  char* buffer = malloc(buffer_size);
  if (buffer == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  // Read until EOF.
  ssize_t bytes_read;
  while (bytes_read = read(STDIN_FILENO, buffer, buffer_size)) {
    if (bytes_read == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    if (write(STDOUT_FILENO, buffer, bytes_read) == -1) {
      perror("write to stdout");
      exit(EXIT_FAILURE);
    }
    if (write(fd, buffer, bytes_read) == -1) {
      fprintf(stderr, "failed to write to %s: %s\n", path, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  free(buffer);

  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

void showUsageAndExit(const char* program_name) {
  fprintf(stderr, "Usage: %s file [-a (append mode)]\n", program_name);
  exit(EXIT_FAILURE);
}

blksize_t blockSizeForIO(int fd) {
  struct stat s;
  if (fstat(fd, &s) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
  return s.st_blksize;
}
