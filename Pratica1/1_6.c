#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[]){
	int l, c;
	int s=0;
	FILE *fp;
	if(argc != 4){
		printf("Numero errado de argumentos.");
		return 0;
	}

	else{

		l=atoi(argv[1]);
		c=atoi(argv[2]);
		int v[l*c];
		printf("Elementos da matriz: \n");
		while(s<(l*c)){
			scanf("%d", &v[s]);
			s++;
		}
	

		fp = fopen(argv[3], "w+");

		fprintf(fp, "%d  %d \n", l, c);

		for(int i=0; i<c*l; i++){
			fprintf(fp, "%d   ", v[i]);
			if((i+1)%c == 0 && i>0)
				fprintf(fp, "\n");
		}
		fclose(fp);
	}



}