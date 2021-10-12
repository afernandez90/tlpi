/**
 * Exercise 6.3
 *
 * Implement setenv() and unsetenv() using getenv(), putenv(), and, where
 * necessary, code that directly modifies environ. Your version of unsetenv()
 * should check to see whether there are multiple definitions of an environment
 * variable, and remove them all (which is what the glibc version of unsetenv()
 * does).
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char** environ;

ssize_t isValidEnvName(const char* s);
size_t environLength();

// See man setenv(3) for details.
int mySetenv(const char* name, const char* value, int overwrite) {
  ssize_t name_len = isValidEnvName(name);
  if (name_len == -1) {
    errno = EINVAL;
    return -1;
  }

  // Search for the first existing entry with the same name.
  char** entry;
  for (entry = environ; *entry != NULL; ++entry) {
    if (strncmp(*entry, name, name_len) == 0 && (*entry)[name_len] == '=') {
      if (overwrite == 0) {
        return 0;
      }
      break;
    }
  }

  // If the entry doesn't exist, make sure the next element is NULL to preserve
  // list property.
  if (*entry == NULL) {
    *(entry + 1) = NULL;
  }

  // Allocate space and populate entry.
  *entry = malloc(name_len + strlen(value) + 2);
  if (*entry == NULL) {
    return -1;
  }
  sprintf(*entry, "%s=%s", name, value);

  return 0;
}

// See man unsetenv(3) for details.
int myUnsetenv(const char* name) {
  ssize_t name_len = isValidEnvName(name);
  if (name_len == -1) {
    errno = EINVAL;
    return -1;
  }

  char** write_ptr = environ;
  for (char** read_ptr = environ; *read_ptr != NULL; ++read_ptr) {
    if (strncmp(*read_ptr, name, name_len) == 0 &&
        (*read_ptr)[name_len] == '=') {
      // Found an entry, don't advance the write pointer.
    } else {
      *write_ptr++ = *read_ptr;
    }
  }

  *write_ptr = NULL;

  return 0;
}

int main() {
  size_t initial_num_vars = environLength();
  printf("Variables in environment at start: %zu\n", initial_num_vars);

  assert(myUnsetenv("Foo") == 0);

  assert(mySetenv("Foo", "InitialValue", 0 /* overwrite */) == 0);
  assert(mySetenv("Foo", "AnotherValue", 0 /* overwrite */) == 0);
  assert(strcmp(getenv("Foo"), "InitialValue") == 0);

  assert(mySetenv("Foo", "FooValue", 1 /* overwrite */) == 0);
  assert(strcmp(getenv("Foo"), "FooValue") == 0);

  assert(mySetenv("", "", 0 /* overwrite */) == -1);
  assert(errno == EINVAL);

  assert(mySetenv("Foo=", "", 0 /* overwrite */) == -1);
  assert(errno == EINVAL);

  // Test prefix detection.
  assert(mySetenv("FooBar", "X", 0 /* overwrite */) == 0);
  assert(mySetenv("Foo", "FooValue", 1 /* overwrite */) == 0);
  assert(strcmp(getenv("FooBar"), "X") == 0);

  assert(myUnsetenv("Foo") == 0);
  assert(myUnsetenv("FooBar") == 0);
  assert(getenv("Foo") == NULL);
  assert(getenv("FooBar") == NULL);

  size_t final_num_vars = environLength();
  assert(final_num_vars == initial_num_vars);

  printf("Variables in environment at end: %zu\n", final_num_vars);
}

// Returns -1 if s is not a valid environment variable name. Otherwise it
// returns its length. Instead of using strchr, we implement the search for
// '=' here to save the length of the string. Saves callers further calls to
// strlen.
ssize_t isValidEnvName(const char* s) {
  if (s == NULL || *s == '\0') {
    return -1;
  }

  const char* c;
  for (c = s; *c != '\0'; ++c) {
    if (*c == '=') {
      return -1;
    }
  }

  return c - s;
}

// Number of elements in the global environ variable.
size_t environLength() {
  char** entry = environ;
  for (entry = environ; *entry != NULL; ++entry) {
  }
  return entry - environ;
}
