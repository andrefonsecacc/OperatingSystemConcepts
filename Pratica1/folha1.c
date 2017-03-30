#include <stdio.h>

/***
    Exercicio 1
    P é um apontador cujo valor é um endereço de memoria
***/

int main (){
  int i=5;
  /*variavel p guarda o endereço de memoria da variavel i*/
  int *p=&i;  
  printf("Valor inicial da variavel p:%d\n", *p);
  p++;
  printf("Valor da variavel p apos incremento p++:%d\n", *p);
  //segmentation fault
  //*p++=0;
  // printf("Valor da variavel p apost incremento *p++:%d\n", *p);

  /***
      Exercicio 2
  ***/
  char c, *cptr;
  c = 'a';
  cptr = &c;
  printf("tamanho de um char: %lu\n", sizeof(c));
  printf("tamanho do endereço de um char: %lu\n", sizeof(&c));
  printf("tamanho de um apontador para um char: %lu\n", sizeof(cptr));
  printf("tamanho do conteúdo apontado por um apontador para um char: %lu\n", sizeof(*cptr));
  printf("Os valores apontados pelos endereços '%p' e '%p' são '%c' e '%c'\n", &c, cptr, c, *cptr);

  printf("Agora para inteiros\n");
  
  // para int
  int d, *dptr;
  d = 10;
  dptr = &d;
  printf("tamanho de um Int: %lu\n", sizeof(d));
  printf("tamanho do endereço de um INT: %lu\n", sizeof(&d));
  printf("tamanho de um apontador para um INT: %lu\n", sizeof(dptr));
  printf("tamanho do conteúdo apontado por um apontador para um INT: %lu\n", sizeof(*dptr));
  printf("Os valores apontados pelos endereços '%p' e '%p' são '%d' e '%d'\n", &d, dptr, d, *dptr);

  /***
      Exercicio 3
      
  ***/
  printf("\n\n");
  int x[3] = {23,41,17};
  printf("x[0] = %d\n",x[0]);
  printf("x[1] = %d\n",x[1]);
  printf("x[2] = %d\n",x[2]);
  printf("*x = %d\n",*x);
  printf("*x+1 = %d\n",*x+1);
  printf("*&(x[0]) = %d\n",*&(x[0]));
}
