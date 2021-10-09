/**
 * Exercise 5.4
 *
 * Implement dup() and dup2() using fcntl() and, where necessary, close(). (You
 * may ignore the fact that dup2() and fcntl() return different errno values for
 * some error cases.) For dup2(), remember to handle the special case where
 * oldfd equals newfd. In this case, you should check whether oldfd is valid,
 * which can be done by, for example, checking if fcntl(oldfd, F_GETFL)
 * succeeds. If oldfd is not valid, then the function should return –1 with
 * errno set to EBADF.
 */
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int myDup(int oldfd) { return fcntl(oldfd, F_DUPFD, 0); }

int myDup2(int oldfd, int newfd) {
  if (oldfd == newfd) {
    if (fcntl(oldfd, F_GETFL) == -1) {
      errno = EBADF;
      return -1;
    }
    return oldfd;
  }

  // Ignore failures.
  close(newfd);

  return fcntl(oldfd, F_DUPFD, newfd);
}

int main() {
  int fd_cout = fileno(stdout);
  printf("myDup(%d) = %d\n", fd_cout, myDup(fd_cout));

  assert(myDup(-1) == -1);
  perror("myDup");

  assert(myDup2(-1, -1) == -1);
  perror("myDup2");

  int fd_cin = fileno(stdin);
  assert(myDup2(fd_cin, fd_cin) == fd_cin);

  printf("myDup2(%d, 100) = %d\n", fd_cin, myDup2(fd_cin, 100));
}
