#include <stdio.h>
#include <stdlib.h>

int add(int a, int b) {
  return a + b;
}

int *alloc_matrix(int m, int n) {
  return (int *) malloc(m * n * sizeof(int));
}

int main() {
  int *mp;
  int (*fun)(int, int);

  mp = alloc_matrix(3, 5);

  // Notação tradicional
  fun = &add;
  printf("2 + 3 = %d\n", (*fun)(2, 3));

  // Notação abreviada
  fun = add;
  printf("4 + 5 = %d\n", fun(4, 5));
  return 0;
}
