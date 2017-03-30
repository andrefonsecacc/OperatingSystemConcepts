#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXARGS 100
typedef struct command {
  char *cmd;              // string apenas com o comando
  int argc;               // número de argumentos
  char *argv[MAXARGS+1];  // vector de argumentos do comando
} COMMAND;

COMMAND parse(COMMAND *com ,char *linha);
void print_parse(COMMAND *com);

int main() {
  char *linha;
  COMMAND *com; 
  com= malloc(sizeof(COMMAND));

  while (1) {
    if ((linha = readline("my_prompt$ ")) == NULL)
      exit(0);
    if (strlen(linha) != 0) {
      add_history(linha);
      parse(com,linha);
      print_parse(com);
    }
    free(linha);
  }
}

COMMAND parse(COMMAND *com ,char *linha) {
  int i;
  // COMMAND *com;
  com->argv[0]=strtok(linha," "); // encontra primeira ocorrencia de " "
  com->cmd=com->argv[0]; // actualiza a linha de comandos
  i=1;
  while ( (com->argv[i]=strtok(NULL, " ")) != NULL){
    com->argc=++i; //actualiza numero de argumentos (ja temos 1 argumento q e' o do comando)       
  }
  com->argv[i]=NULL;
  // return com;
}

void print_parse(COMMAND *com) {
  int comp = strcmp(com->cmd,"exit");
  if(comp==0){
    printf("Exiting the program....\n");
    exit(0);
  }
  else{
    int k;
    printf("COMMAND: %s\n",com->cmd);
    if(com->argc >= 1){printf("Argv: ");}
    for(k=1;k<com->argc;k++){
      printf( "%s\n",com->argv[k]);
    }
    printf("\n");
      
  }
   
}
