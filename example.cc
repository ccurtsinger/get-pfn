#include "pagemap.hh"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int x;
  pagemap map(getpid());
  printf("x is at virtual address %p\n", &x);
  printf("x is at physical address %p\n", map[&x]);
  return 0;
}

