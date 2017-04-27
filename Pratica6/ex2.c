#include <stdio.h>
  #include <stdlib.h>

  long long *create_vector1(int);
  long long *create_vector2(int);
  void do_work(void);
  void produto_interno(int, int);

  int threads, vector_size;
  long long prod_int, *vector_a, *vector_b;

  int main(int argc, char *argv[]) {
    vector_size = atoi(argv[1]);
    threads = atoi(argv[2]);
    vector_a = create_vector1(vector_size);
    vector_b = create_vector2(vector_size);
    do_work();
    return EXIT_SUCCESS;
  }

  long long *create_vector1(int size) {
    int i;
    long long *vector;

    vector = (long long *) malloc(size * sizeof(long long));
    for (i = 0; i < size; i++)
      vector[i] = i;	
    return vector;
  }

  long long *create_vector2(int size) {
    int i;
    long long *vector;

   vector = (long long *) malloc(size * sizeof(long long));
    for (i = 0; i < size; i++)
      vector[i] = size-i-1;	
    return vector;
  }

  void do_work(void) {
    prod_int = 0;
    produto_interno(0, vector_size);
    printf("Produto Interno = %lld\n", prod_int);
    return;
  }

  void produto_interno(int start, int end) {
    int i;

    for (i = start; i < end; i++) 
      prod_int += vector_a[i] * vector_b[i];
    return;
  }
