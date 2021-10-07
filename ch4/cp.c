/**
 * Exercise 4.2
 *
 * Write a program like `cp` that, when used to copy a regular file that
 * contains holes (sequences of null bytes), also creates corresponding holes in
 * the target file.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void showUsageAndExit(const char* program_name);
int openFile(const char* path, int flags);
mode_t filePermissions(const char* path);
void setPermissions(const char* path, mode_t mode);
blksize_t blockSizeForIO(const char* path);
void closeFileDescriptor(int fd);

int main(int argc, char** argv) {
  if (argc != 3) {
    showUsageAndExit(argv[0]);
  }

  const char* src_path = argv[1];
  const char* dst_path = argv[2];

  int fd_src = openFile(src_path, O_RDONLY);

  // Create/truncate destination file and set the same permissions as the
  // source.
  int fd_dst = openFile(dst_path, O_CREAT | O_TRUNC | O_WRONLY);
  setPermissions(dst_path, filePermissions(src_path));

  // Allocate write buffer.
  blksize_t buffer_size = blockSizeForIO(dst_path);
  char* buffer = malloc(buffer_size);
  if (buffer == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  // Keep reading until EOF.
  ssize_t bytes_read;
  while (bytes_read = read(fd_src, buffer, buffer_size)) {
    if (bytes_read == -1) {
      perror("read");
      exit(EXIT_FAILURE);
    }

    // Write buffered data to destination. If there is at least one non-null
    // byte in the buffer, write the whole buffer as is. Seeking past EOF is
    // only worth it if the size of the hole is bigger than the output file's
    // block size. Otherwise blocks are created anyway with explicit holes and
    // we just waste resources with unnecessary `lseek()` and `write()`
    // syscalls instead of a single `write()` for the whole block).
    char is_hole = 1;
    for (int i = 0; i < bytes_read; ++i) {
      if (buffer[i] != 0) {
        is_hole = 0;
        if (write(fd_dst, buffer, bytes_read) == -1) {
          perror("write");
          exit(EXIT_FAILURE);
        }
        break;
      }
    }

    if (is_hole) {
      lseek(fd_dst, bytes_read, SEEK_CUR);
    }
  }

  free(buffer);

  closeFileDescriptor(fd_src);
  closeFileDescriptor(fd_dst);

  exit(EXIT_SUCCESS);
}

void showUsageAndExit(const char* program_name) {
  fprintf(stderr, "Usage: %s file [-a (append mode)]\n", program_name);
  exit(EXIT_FAILURE);
}

int openFile(const char* path, int flags) {
  int fd = open(path, flags);
  if (fd == -1) {
    fprintf(stderr, "failed to open %s: %s\n", path, strerror(errno));
  }
}

void fileStatus(const char* path, struct stat* st_out) {
  if (stat(path, st_out) == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }
}

mode_t filePermissions(const char* path) {
  struct stat s;
  fileStatus(path, &s);
  return s.st_mode;
}

void setPermissions(const char* path, mode_t mode) {
  if (chmod(path, mode) == -1) {
    perror("chmod");
    exit(EXIT_FAILURE);
  }
}

blksize_t blockSizeForIO(const char* path) {
  struct stat s;
  fileStatus(path, &s);
  return s.st_blksize;
}

void closeFileDescriptor(int fd) {
  if (close(fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
}
