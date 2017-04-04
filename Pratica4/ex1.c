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

    char hello[]="Hello World!";
    char bye[]="Goodbye!";
    char readbuffer[100];

    pid_t pid;

    pipe(fd);
    pid=fork();
    
    if(pid==0){ //filho
        close(fd[WRITE]);
	//sleep(3);
        read(fd[READ], readbuffer, sizeof(readbuffer));
        printf("Msg Recebida-> %s \n", readbuffer);
        sleep(6);
        read(fd[READ], readbuffer, sizeof(readbuffer));
        printf("Msg Recebida-> %s \n", readbuffer);
    }
    else{       //pai
      //sleep(3);
        close(fd[READ]);
        write(fd[WRITE], hello, (strlen(hello)+1));
	printf("Verificação : Mensagem enviada com sucesso pelo Pai!\n");
	 sleep(3);
        write(fd[WRITE], bye, (strlen(bye)+1));
	printf("Verificação:Mensagem enviada com sucesso pelo Pai!\n");
        wait(&status);
        if(WIFEXITED(status)){
            printf("Concluido com sucesso\n");
	    //dbuffer[100];
	}
    }
}
