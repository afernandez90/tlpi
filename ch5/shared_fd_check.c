/**
 * Exercise 5.5
 *
 * Write a program to verify that duplicated file descriptors share a file
 * offset value and open file status flags.
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void printInfo(int orig_fd, int dup_fd);

int main() {
  // Create a temporary file.
  int fd;
  {
    FILE* tmp_file = tmpfile();
    if (tmp_file == NULL) {
      perror("tmpfile");
      exit(EXIT_FAILURE);
    }
    fd = tmp_file->_fileno;
  }

  // Duplicate its file descriptor.
  int dup_fd = dup(fd);
  if (dup_fd == -1) {
    perror("dup");
    exit(EXIT_FAILURE);
  }

  printf("original state:\n");
  printInfo(fd, dup_fd);
  printf("-----------------\n");

  if (write(fd, "foo", 3) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  printf("State after writing on original descriptor:\n");
  printInfo(fd, dup_fd);
  printf("-----------------\n");

  if (write(dup_fd, "bar", 3) == -1) {
    perror("write");
    exit(EXIT_FAILURE);
  }
  printf("State after writing on duplicate descriptor:\n");
  printInfo(fd, dup_fd);
  printf("-----------------\n");

  if (fcntl(fd, F_SETFL, O_APPEND) == -1) {
    perror("fcntl");
    exit(EXIT_FAILURE);
  }
  printf("State after setting O_APPEND flag on original descriptor:\n");
  printInfo(fd, dup_fd);
  printf("-----------------\n");

  if (fcntl(dup_fd, F_SETFL, 0) == -1) {
    perror("fcntl");
    exit(EXIT_FAILURE);
  }
  printf("State after clearing all flags on original descriptor:\n");
  printInfo(fd, dup_fd);

  exit(EXIT_SUCCESS);
}

void printInfo(int orig_fd, int dup_fd) {
  printf(
      "orig_fd: %d\n"
      "dup_fd: %d\n"
      "orig_fd flags: %d\n"
      "dup_fd flags: %d\n"
      "orig_fd offset: %zd\n"
      "dup_fd offset: %zd\n",
      orig_fd, dup_fd, fcntl(orig_fd, F_GETFL), fcntl(dup_fd, F_GETFL),
      lseek(orig_fd, 0, SEEK_CUR), lseek(dup_fd, 0, SEEK_CUR));
}
