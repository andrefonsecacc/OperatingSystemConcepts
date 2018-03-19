#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
 
#define PIPE_READ 0
#define PIPE_WRITE 1



int main(int argc, char const *argv[])
{
	int fd[2], pid;

	if(pipe(fd)==-1){
		printf("Erro na criação da Pipe\n");
	}

	if((pid=fork())==-1){
		printf("Erro na criação do processo filho\n");
	}
	//pai
	if(pid){
		close(fd[PIPE_WRITE]);
		
          if(argv[1]!=NULL){
              int infd=open(argv[1], O_RDONLY);
              if(infd<0){ 
                  perror("Error opening file decriptor to file");
                    exit(1);
            }
            dup2(infd, 0);		// valor 0 corresponde ao valor do STDIN
            close(fd[PIPE_READ]);
            if (execlp("cat","cat",NULL)<0)
            {
            	/* exec error */
            }
			
		}
	}
	//filho
	else{
		close(fd[PIPE_WRITE]);
		dup2(fd[PIPE_READ],STDIN_FILENO);// to the stdin
		
		if(argv[2]!=NULL){
			int outfd=open(argv[2], O_WRONLY|O_CREAT|O_APPEND, 0777);		//0777 da modo leitura, escrita e executavel
       		dup2(outfd, 1);		// valor 1 corresponde ao valor do STDOUT
		}
		execlp("grep","grep",argv[3]);
		perror("Execution failed");
	}

	return 0;
}