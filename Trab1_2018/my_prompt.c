/////////////////////////////////////////////////////////
//                                                     //
//               Trabalho I: Mini-Shell                //
//                                                     //
// Compilação: gcc my_prompt.c -lreadline -o my_prompt //
// Utilização: ./my_prompt                             //
//                                                     //
/////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#define MAXARGS 100
#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct command {
    char *cmd;              // string apenas com o comando
    int argc;               // número de argumentos
    char *argv[MAXARGS+1];  // vector de argumentos do comando
    struct command *next;   // apontador para o próximo comando
} COMMAND;

// variáveis globais
char* inputfile = NULL;	    // nome de ficheiro (em caso de redireccionamento da entrada padrão)
char* outputfile = NULL;    // nome de ficheiro (em caso de redireccionamento da saída padrão)
int background_exec = 0;    // indicação de execução concorrente com a mini-shell (0/1)

// declaração de funções
COMMAND* parse(char *);
void print_parse(COMMAND *);
void execute_commands(COMMAND *);
void free_commlist(COMMAND *);
void task5(COMMAND *);

// include do código do parser da linha de comandos
#include "parser.c"

int main(int argc, char* argv[]) {
  char *linha;
  COMMAND *com;

  while (1) {
    if ((linha = readline("my_prompt$ ")) == NULL)
      exit(0);
    if (strlen(linha) != 0) {
      add_history(linha);
      com = parse(linha);
      if (com) {
	print_parse(com);
	execute_commands(com);
	free_commlist(com);
      }
    }
    free(linha);
  }
}


void print_parse(COMMAND* commlist) {
  int n, i;

  printf("---------------------------------------------------------\n");
  printf("BG: %d IN: %s OUT: %s\n", background_exec, inputfile, outputfile);
  n = 1;
  while (commlist != NULL) {
    printf("#%d: cmd '%s' argc '%d' argv[] '", n, commlist->cmd, commlist->argc);
    i = 0;
    while (commlist->argv[i] != NULL) {
      printf("%s,", commlist->argv[i]);
      i++;
    }
    printf("%s'\n", commlist->argv[i]);
    commlist = commlist->next;
    n++;
  }
  printf("---------------------------------------------------------\n");
}


void free_commlist(COMMAND *commlist){
    // ...
    // Esta função deverá libertar a memória alocada para a lista ligada.
    // ...
   COMMAND* aux = NULL;
    while(commlist=NULL){
      aux=commlist;
      commlist=commlist->next;
      free(aux);
    }
  
}

void task5(COMMAND *commlist){

  pid_t pid;
  pid_t pidB;
  int infd;
  int outfd;

  int fd[2];

        if ((pid=fork())<0){ perror("Process creation failed");}  
        

        if(pid==0){ //filho A
          
        if(pipe(fd)==-1){ printf("Erro na criação da Pipe\n");}

        if((pidB=fork())<0){ perror("Process creation failed"); }

              if(pidB==0){ // filho B
                
                close(fd[PIPE_WRITE]);
                dup2(fd[PIPE_READ],STDIN_FILENO);// to the stdin
              
                if(outputfile!=NULL){
                int outfd=open(outputfile, O_WRONLY|O_CREAT|O_APPEND, 0777);   //0777 da modo leitura, escrita e executavel
                dup2(outfd, 1);   // valor 1 corresponde ao valor do STDOUT
                              }

                execlp("grep","grep",commlist->argv[3]);
                perror("Execution failed");
                wait(NULL);  
            }else{ // pai b

                close(fd[PIPE_WRITE]);
                if(inputfile!=NULL){
                int infd=open(inputfile, O_RDONLY);
                if(infd<0){ 
                    perror("Error opening file decriptor to file");
                    exit(1);}
                dup2(infd, 0);    // valor 0 corresponde ao valor do STDIN
                close(fd[PIPE_READ]);}
                if (execlp("cat","cat",NULL)<0){ /*exec error*/  }                   
                }
        }else{ // pai
          
        }
}



void execute_commands(COMMAND *commlist) {
    // ...
    // Esta função deverá "executar" a "pipeline" de comandos da lista commlist.
    // ...
    pid_t pid;
    int infd, outfd;

    if (commlist->argc<=MAXARGS){
      
        if(strcmp(commlist->cmd,"exit")==0){
          exit(0);
        }

        if(strcmp(commlist->cmd,"filtro")==0){
          task5(commlist);
        }


        if ((pid=fork())<0){
          /* code */
          perror("Process creation failed");
        }

        if (pid==0) //filho
        {
          if(inputfile!=NULL){
            infd=open(inputfile, O_RDONLY);
            if(infd==-1){perror("Error trying to opening file!");}
            
            dup2(infd,STDIN_FILENO);
            close(infd);
          }

          if (outputfile!=NULL){
              outfd=open(outputfile, O_RDWR | O_CREAT | O_TRUNC , 0777);
              if(outfd==-1){ perror("Error creating/reading");}
              
              dup2(outfd,STDOUT_FILENO);
              close(outfd);
          }

          execvp(commlist->cmd,commlist->argv);
          
        }
        else{ //pai
          wait(NULL);
        }
      }
    }
            

