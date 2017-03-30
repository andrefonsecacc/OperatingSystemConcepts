#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){

  FILE *fp;
  int c=0;
  int row, columns;
    
  fp = fopen(argv[1],"r");
  fscanf(fp, "%d %d ",&row,&columns);
  int v[row*columns];
    while(1){
      fscanf(fp,"%d",&v[c]);
      c++;   
      if( feof(fp) )
	{ 
	  break ;
	}
   
    }   
    fclose(fp);
    int k=1;
    for(c=0;c<row*columns;c++){
	printf("%d\t",v[c]);
	if(k%columns==0){
	printf("\n");
      }
	k++;
    }

  return 0;
 
}
