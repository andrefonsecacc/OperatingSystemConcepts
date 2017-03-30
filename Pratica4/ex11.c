#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define WRITE 1
#define READ 0

int main(){
int status;
    int fd[2];
    int n;
    //char hello[]="Hello World!";
    // char bye[]="Goodbye!";
    char hello[10];
    char readbuffer[100];
    char end[3]="end";
    pid_t pid;
    int j;
    pipe(fd);
    pid=fork();
    
    if(pid==0){ //filho

      do{
      close(fd[WRITE]);
	//sleep(3);
        n=read(fd[READ], readbuffer, sizeof(readbuffer));
        printf("Msg Recebida-> %s \n", readbuffer);
	//sleep(6);
	// read(fd[READ], readbuffer, sizeof(readbuffer));
        //printf("Msg Recebida-> %s \n", readbuffer);
      }while(n!=0);
      
    }
    else{       //pai
      //sleep(3);
        close(fd[READ]);

	do{

	  //char hello[10];
	gets(hello);
        j=write(fd[WRITE], hello, (strlen(hello)+1));
	printf("Verificação : Mensagem enviada com sucesso pelo Pai!\n");


	// sleep(3);
	// write(fd[WRITE], bye, (strlen(bye)+1));
	//	printf("Verificação:Mensagem enviada com sucesso pelo Pai!\n");
	}while(j!=0);
        wait(&status);
        if(WIFEXITED(status)){
            printf("Concluido com sucesso\n");
	    //dbuffer[100];
	}
    }
}
