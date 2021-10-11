/**
 * Exercise 5.6
 *
 * After each of the calls to write() in the following code, explain what the
 * content of the output file would be, and why:
 *   fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
 *   fd2 = dup(fd1);
 *   fd3 = open(file, O_RDWR);
 *   write(fd1, "Hello,", 6);
 *   write(fd2, "world!", 6);
 *   lseek(fd2, 0, SEEK_SET);
 *   write(fd1, "HELLO,", 6);
 *   write(fd3, "Gidday", 6);
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  char filename[] = "./explain_write_calls_fileXXXXXX";
  if (mkstemp(filename) == -1) {
    perror("mkstemp");
    exit(EXIT_FAILURE);
  }

  // Opens filename. fd1 refers to entry A in the kernel's FD table.
  int fd1 = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  // Duplicates fd1. fd2 will differ from fd1, but points to A in the kernel's
  // FD table. Therefore, fd1 and fd2 share file flags and offset.
  int fd2 = dup(fd1);
  // Opens filename. fd3 differs from fd1 and fd2 and has its own entry in the
  // kernel's FD table.
  int fd3 = open(filename, O_RDWR);

  // Filename contents: "Hello,".
  write(fd1, "Hello,", 6);
  // Filename contents: "Hello,world!".
  write(fd2, "world!", 6);
  // The offset of file descriptor A in the kernel's FD table is set to 0.
  lseek(fd2, 0, SEEK_SET);
  // Filename contents: "HELLO,world!".
  write(fd1, "HELLO,", 6);
  // Filename contents: "Giddayworld!".
  write(fd3, "Gidday", 6);
}
