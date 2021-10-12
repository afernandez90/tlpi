/**
 * Exercise 6.2
 *
 * Write a program to see what happens if we try to longjmp() into a function
 * that has already returned.
 */

#include <setjmp.h>
#include <stdlib.h>

void f(jmp_buf* env) { setjmp(*env); }

int main() {
  jmp_buf env;
  f(&env);
  longjmp(env, 1);  // Segfault
  exit(EXIT_SUCCESS);
}
