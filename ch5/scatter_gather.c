/**
 * Exercise 5.7
 *
 * Implement readv() and writev() using read(), write(), and suitable functions
 * from the malloc package (Section 7.1.2).
 *
 * No need to use malloc if the buffers are allocated on the stack.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t myReadv(int fd, const struct iovec *iov, int iovcnt) {
  ssize_t total_bytes_read = 0;

  for (int i = 0; i < iovcnt; ++i) {
    const struct iovec *current = &iov[i];
    ssize_t bytes_read = read(fd, current->iov_base, current->iov_len);

    // Check for errors.
    if (bytes_read == -1) {
      return -1;
    }

    // If we read less bytes than requested, we're done.
    total_bytes_read += bytes_read;
    if (bytes_read < current->iov_len) {
      break;
    }
  }

  return total_bytes_read;
}

ssize_t myWritev(int fd, const struct iovec *iov, int iovcnt) {
  ssize_t total_bytes_written = 0;

  for (int i = 0; i < iovcnt; ++i) {
    const struct iovec *current = &iov[i];
    ssize_t bytes_written = write(fd, current->iov_base, current->iov_len);

    // Check for errors.
    if (bytes_written == -1) {
      return -1;
    }
  }

  return total_bytes_written;
}

int main() {
  char header[5];
  char payload[12];
  struct iovec iov[2];

  iov[0].iov_base = &header;
  iov[0].iov_len = sizeof(header);

  iov[1].iov_base = &payload;
  iov[1].iov_len = sizeof(payload);

  if (myReadv(STDIN_FILENO, iov, 2) == -1) {
    perror("myReadv");
    exit(EXIT_FAILURE);
  }

  if (myWritev(STDOUT_FILENO, iov, 2) == -1) {
    perror("myWritev");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
