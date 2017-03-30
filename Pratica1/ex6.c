#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bubble_sort(int list[], int n){
  int c, d, t;

  for (c = 0 ; c < ( n - 1 ); c++){
    for (d = 0 ; d < n - c - 1; d++){
	  if (list[d] > list[d+1]){
	      /* Swapping */
	      t         = list[d];
	      list[d]   = list[d+1];
	      list[d+1] = t;
	    }
	}
    }
}

int main(int argc, char *agrv[]){
  int m,n;
  int s =0;
  FILE *fp;
  if (argc >4){
    printf ("Demasiados argumentos ou a menos....\n");
    return 0;
  }
  else{
    m = atoi(agrv[1]);
    n = atoi(agrv[2]);
    printf("Inserir numeros:");
    int v[m*n];
    while(s<(m*n)){
      scanf("%d",&v[s]);
      s++;
    }
    bubble_sort(v,(n*m));
    /***
    para imprimir so
    int c;
    for ( c = 0 ; c < n*m ; c++ )
    printf("%d\n", v[c]);
***/

    fp = fopen(agrv[3],"w+");
    int c;
    fprintf(fp,"%d %d ",m,n);
    for ( c = 0 ; c < n*m ; c++ ){
      fprintf(fp," %d ",v[c]);
    }
    fclose(fp);
  }
}
