/////////////////////////////////////////////////////////
//                                                     //
//               Trabalho I: Mini-Shell                //
//                                                     //
// Compilação: gcc my_prompt.c -lreadline -o my_prompt //
// Utilização: ./my_prompt                             //
//                                                     //
/////////////////////////////////////////////////////////
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
    COMMAND* parse(char* linha);
    void print_parse(COMMAND* commlist);
    void execute_commands(COMMAND* commlist);
    void free_commlist(COMMAND* commlist);

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
       //Tarefa1
       //free(inputfile); não vale a pena
       //free(outputfile); não vale a pena
       COMMAND* aux;
       while(commlist!=NULL){
        aux = commlist;
        commlist = commlist->next;
        free(aux);
      }

    }
    void execute_commands(COMMAND *commlist) {
        // ...
        // Esta função deverá "executar" a "pipeline" de comandos da lista commlist.
        // Tarefa2
      int first_child = -1; 
      int n=0;
      int status;
      pid_t pid;

      while(commlist!=NULL){
          int fd[2];
        //Tarefa 5 
        if (pipe(fd)==-1){
          perror("Pipe creation failed");
          exit(1);
        }
        if(strcmp(commlist->cmd,"exit")==0){
        exit(1);
          }  
          
        //pid =fork();
        if((pid=fork())==0){ //filho

        if(n==0 && commlist->next!=NULL){
            close(fd[PIPE_READ]);
            dup2(fd[PIPE_WRITE],STDOUT_FILENO); // STDOUT_FILENO File number of stdout; 1.
            close(fd[PIPE_WRITE]);
          }
          else if(commlist->next!=NULL){
            close(fd[PIPE_READ]);
            dup2(first_child,STDIN_FILENO);
            dup2(fd[PIPE_WRITE],STDOUT_FILENO);
	    close(first_child);
            close(fd[PIPE_WRITE]);
          }
          else if(commlist->next==NULL && n>0){
              dup2(first_child,STDIN_FILENO);
	      close(first_child);
          }

          //Tarefa 3
          //encontra ficheiro de input e testa se e primeiro filho
          if(inputfile!=NULL && commlist->next==NULL){
	    //file decriptor inputfile so 1 posicao visto so alterarmos a entrada padrao
              int infd=open(inputfile, O_RDONLY);
              if(infd<0){ 
                  perror("Error opening file decriptor to file");
                    exit(1);
            }
            dup2(infd, 0);		// valor 0 corresponde ao valor do STDIN
            close(infd);

          }
          //Tarefa 4
          //encontra ficheiro de output e testa se e ultimo filho
          if(outputfile!=NULL && commlist->next==NULL){
        //file decriptor outputfile so 1 posicao visto so alterarmos a entrada padrao
        int outfd=open(outputfile, O_WRONLY|O_CREAT|O_APPEND, 0777);		//0777 da modo leitura, escrita e executavel
        dup2(outfd, 1);		// valor 1 corresponde ao valor do STDOUT
          }
        execvp(commlist->cmd, commlist->argv);
        perror("Execution failed");
        exit(1);

      }
      else{ //pai
        //first_child=0;

        if(background_exec==0){
          waitpid(pid,&status,WUNTRACED | WCONTINUED); //processo pai espera que o filho termine para apresentar o texto "myprompt"
        }
        if(commlist->next!=NULL){
          close(fd[PIPE_WRITE]);
          first_child=fd[PIPE_READ];
        }
        commlist=commlist->next;
        n++;
      }
    } 
    }

