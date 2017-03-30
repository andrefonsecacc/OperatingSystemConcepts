#include <stdio.h>
#include <stdlib.h>


int add(int a, int b) {
  return a + b;
}

//reserva memoria dinamicamente
int *alloc_matrix(int m, int n) {
  return (int *) malloc(m * n * sizeof(int));
}

int main() {

  //apontador para o alocador de memoria(aponta pro alloc_matrix)
  int *mp;
  //aponta pro add
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
