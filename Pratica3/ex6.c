#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
 
int main(){
    pid_t p;
    p=fork();
    int pidfilho=0;
    int pidpai=0;
    int dif;
    int x;
    if(p==0){   //e' filho
        pidfilho=getpid();
        pidpai=getppid();       
        dif=pidfilho-pidpai;     // inteiro do status 
	printf("Filho: PID %d , PPID %d ,UID %d\n",getpid(),getppid(),getuid());
        printf("Diferenca de PID-> %d \n",dif);
    exit(dif);
         
    }
    else{       //e' pai
      wait(&x);// ese
        if(WIFEXITED(x)){
	  printf("PAI: PID %d ,PPID %d ,UID %d\n ",getpid(),getppid(),getuid());
            printf("diferenca-> %d\n", WIFEXITED(x));      
        }   
    }
    return 0;
}
