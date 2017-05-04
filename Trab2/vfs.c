////////////////////////////////////////////////////////////////////////
//                                                                    //
//            Trabalho II: Sistema de Gestão de Ficheiros             //
//                                                                    //
// Compilação: gcc vfs.c -Wall -lreadline -o vfs                      //
// Utilização: ./vfs [-b[128|256|512|1024]] [-f[7|8|9|10]] FILESYSTEM //
//                                                                    //
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXARGS 100
#define CHECK_NUMBER 9999
#define TYPE_DIR 'D'
#define TYPE_FILE 'F'
#define MAX_NAME_LENGHT 20

#define FAT_ENTRIES(TYPE) ((TYPE) == 7 ? 128 : (TYPE) == 8 ? 256 : (TYPE) == 9 ? 512 : 1024)
#define FAT_SIZE(TYPE) (FAT_ENTRIES(TYPE) * sizeof(int))
#define BLOCK(N) (blocks + (N) * sb->block_size)
#define DIR_ENTRIES_PER_BLOCK (sb->block_size / sizeof(dir_entry))

typedef struct command {
  char *cmd;              // string apenas com o comando
  int argc;               // número de argumentos
  char *argv[MAXARGS+1];  // vector de argumentos do comando
} COMMAND;

typedef struct superblock_entry {
  int check_number;   // número que permite identificar o sistema como válido
  int block_size;     // tamanho de um bloco {128, 256 (default), 512 ou 1024 bytes}
  int fat_type;       // tipo de FAT {7, 8 (default), 9 ou 10}
  int root_block;     // número do 1º bloco a que corresponde o diretório raiz
  int free_block;     // número do 1º bloco da lista de blocos não utilizados
  int n_free_blocks;  // total de blocos não utilizados
} superblock;

typedef struct directory_entry {
  char type;                   // tipo da entrada (TYPE_DIR ou TYPE_FILE)
  char name[MAX_NAME_LENGHT];  // nome da entrada
  unsigned char day;           // dia em que foi criada (entre 1 e 31)
  unsigned char month;         // mes em que foi criada (entre 1 e 12)
  unsigned char year;          // ano em que foi criada (entre 0 e 255 - 0 representa o ano de 1900)
  int size;                    // tamanho em bytes (0 se TYPE_DIR)
  int first_block;             // primeiro bloco de dados
} dir_entry;

// variáveis globais
superblock *sb;   // superblock do sistema de ficheiros
int *fat;         // apontador para a FAT
char *blocks;     // apontador para a região dos dados
int current_dir;  // bloco do diretório corrente

// funções auxiliares
COMMAND parse(char *);
void parse_argv(int, char **);
void show_usage_and_exit(void);
void init_filesystem(int, int, char *);
void init_superblock(int, int);
void init_fat(void);
void init_dir_block(int, int);
void init_dir_entry(dir_entry *, char, char *, int, int);
void exec_com(COMMAND);
int get_free_block();
void vfs_pwd_aux(dir_entry * entry);
void put_free_block(int nome);

// funções de manipulação de diretórios
void vfs_ls(void);
void vfs_mkdir(char *);
void vfs_cd(char *);
void vfs_pwd(void);
void vfs_rmdir(char *);

// funções de manipulação de ficheiros
void vfs_get(char *, char *);
void vfs_put(char *, char *);
void vfs_cat(char *);
void vfs_cp(char *, char *);
void vfs_mv(char *, char *);
void vfs_rm(char *);


int main(int argc, char *argv[]) {
  char *linha;
  COMMAND com;

  parse_argv(argc, argv);
  while (1) {
    if ((linha = readline("vfs$ ")) == NULL)
      exit(0);
    if (strlen(linha) != 0) {
      add_history(linha);
      com = parse(linha);
      exec_com(com);
    }
    free(linha);
  }
  return 0;
}


COMMAND parse(char *linha) {
  int i = 0;
  COMMAND com;

  com.cmd = strtok(linha, " ");
  com.argv[0] = com.cmd;
  while ((com.argv[++i] = strtok(NULL, " ")) != NULL);
  com.argc = i;
  return com;
}


void parse_argv(int argc, char *argv[]) {
  int i, block_size, fat_type;

  // valores por omissão
  block_size = 256;
  fat_type = 8;
  if (argc < 2 || argc > 4) {
    printf("vfs: invalid number of arguments\n");
    show_usage_and_exit();
  }
  for (i = 1; i < argc - 1; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'b') {
	block_size = atoi(&argv[i][2]);
	if (block_size != 128 && block_size != 256 && block_size != 512 && block_size != 1024) {
	  printf("vfs: invalid block size (%d)\n", block_size);
	  show_usage_and_exit();
	}
      } else if (argv[i][1] == 'f') {
	fat_type = atoi(&argv[i][2]);
	if (fat_type != 7 && fat_type != 8 && fat_type != 9 && fat_type != 10) {
	  printf("vfs: invalid fat type (%d)\n", fat_type);
	  show_usage_and_exit();
	}
      } else {
	printf("vfs: invalid argument (%s)\n", argv[i]);
	show_usage_and_exit();
      }
    } else {
      printf("vfs: invalid argument (%s)\n", argv[i]);
      show_usage_and_exit();
    }
  }
  init_filesystem(block_size, fat_type, argv[argc-1]);
  return;
}


void show_usage_and_exit(void) {
  printf("Usage: vfs [-b[128|256|512|1024]] [-f[7|8|9|10]] FILESYSTEM\n");
  exit(1);
}


void init_filesystem(int block_size, int fat_type, char *filesystem_name) {
  int fsd, filesystem_size;

  if ((fsd = open(filesystem_name, O_RDWR)) == -1) {
    // o sistema de ficheiros não existe --> é necessário criá-lo e formatá-lo
    if ((fsd = open(filesystem_name, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU)) == -1) {
      printf("vfs: cannot create filesystem (%s)\n", filesystem_name);
      show_usage_and_exit();
    }

    // calcula o tamanho do sistema de ficheiros
    filesystem_size = block_size + FAT_SIZE(fat_type) + FAT_ENTRIES(fat_type) * block_size;
    printf("vfs: formatting virtual file-system (%d bytes) ... please wait\n", filesystem_size);

    // estende o sistema de ficheiros para o tamanho desejado
    lseek(fsd, filesystem_size - 1, SEEK_SET);
    write(fsd, "", 1);

    // faz o mapeamento do sistema de ficheiros e inicia as variáveis globais
    if ((sb = (superblock *) mmap(NULL, filesystem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fsd, 0)) == MAP_FAILED) {
      close(fsd);
      printf("vfs: cannot map filesystem (mmap error)\n");
      exit(1);
    }
    fat = (int *) ((unsigned long int) sb + block_size);
    blocks = (char *) ((unsigned long int) fat + FAT_SIZE(fat_type));
    
    // inicia o superblock
    init_superblock(block_size, fat_type);
    
    // inicia a FAT
    init_fat();
    
    // inicia o bloco do diretório raiz '/'
    init_dir_block(sb->root_block, sb->root_block);
  } else {
    // calcula o tamanho do sistema de ficheiros
    struct stat buf;
    stat(filesystem_name, &buf);
    filesystem_size = buf.st_size;

    // faz o mapeamento do sistema de ficheiros e inicia as variáveis globais
    if ((sb = (superblock *) mmap(NULL, filesystem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fsd, 0)) == MAP_FAILED) {
      close(fsd);
      printf("vfs: cannot map filesystem (mmap error)\n");
      exit(1);
    }
    fat = (int *) ((unsigned long int) sb + sb->block_size);
    blocks = (char *) ((unsigned long int) fat + FAT_SIZE(sb->fat_type));

    // testa se o sistema de ficheiros é válido 
    if (sb->check_number != CHECK_NUMBER || filesystem_size != sb->block_size + FAT_SIZE(sb->fat_type) + FAT_ENTRIES(sb->fat_type) * sb->block_size) {
      munmap(sb, filesystem_size);
      close(fsd);
      printf("vfs: invalid filesystem (%s)\n", filesystem_name);
      show_usage_and_exit();
    }
  }
  close(fsd);

  // inicia o diretório corrente
  current_dir = sb->root_block;
  return;
}


void init_superblock(int block_size, int fat_type) {
  sb->check_number = CHECK_NUMBER;
  sb->block_size = block_size;
  sb->fat_type = fat_type;
  sb->root_block = 0;
  sb->free_block = 1;
  sb->n_free_blocks = FAT_ENTRIES(fat_type) - 1;
  return;
}


void init_fat(void) {
  int i;

  fat[0] = -1;
  for (i = 1; i < sb->n_free_blocks; i++)
    fat[i] = i + 1;
  fat[sb->n_free_blocks] = -1;
  return;
}


void init_dir_block(int block, int parent_block) {
  dir_entry *dir = (dir_entry *) BLOCK(block);
  // o número de entradas no diretório (inicialmente 2) fica guardado no campo size da entrada "."
  init_dir_entry(&dir[0], TYPE_DIR, ".", 2, block);
  init_dir_entry(&dir[1], TYPE_DIR, "..", 0, parent_block);
  return;
}


void init_dir_entry(dir_entry *dir, char type, char *name, int size, int first_block) {
  time_t cur_time = time(NULL);
  struct tm *cur_tm = localtime(&cur_time);

  dir->type = type;
  strcpy(dir->name, name);
  dir->day = cur_tm->tm_mday;
  dir->month = cur_tm->tm_mon + 1;
  dir->year = cur_tm->tm_year;
  dir->size = size;
  dir->first_block = first_block;
  return;
}


void exec_com(COMMAND com) {
  // para cada comando invocar a função que o implementa
  if (!strcmp(com.cmd, "exit")) {
    exit(0);
  } else if (!strcmp(com.cmd, "ls")) {
    if (com.argc > 1)
      printf("ERROR(input: 'ls' - too many arguments)\n");
    else
      vfs_ls();
  } else if (!strcmp(com.cmd, "mkdir")) {
    if (com.argc < 2)
      printf("ERROR(input: 'mkdir' - too few arguments)\n");
    else if (com.argc > 2)
      printf("ERROR(input: 'mkdir' - too many arguments)\n");
    else
      vfs_mkdir(com.argv[1]);
  } else if (!strcmp(com.cmd, "cd")) {
    if (com.argc < 2)
      printf("ERROR(input: 'cd' - too few arguments)\n");
    else if (com.argc > 2)
      printf("ERROR(input: 'cd' - too many arguments)\n");
    else
      vfs_cd(com.argv[1]);
  } else if (!strcmp(com.cmd, "pwd")) {
    if (com.argc != 1)
      printf("ERROR(input: 'pwd' - too many arguments)\n");
    else
      vfs_pwd();
  } else if (!strcmp(com.cmd, "rmdir")) {
    if (com.argc < 2)
      printf("ERROR(input: 'rmdir' - too few arguments)\n");
    else if (com.argc > 2)
      printf("ERROR(input: 'rmdir' - too many arguments)\n");
    else
      vfs_rmdir(com.argv[1]);
  } else if (!strcmp(com.cmd, "get")) {
    if (com.argc < 3)
      printf("ERROR(input: 'get' - too few arguments)\n");
    else if (com.argc > 3)
      printf("ERROR(input: 'get' - too many arguments)\n");
    else
      vfs_get(com.argv[1], com.argv[2]);
  } else if (!strcmp(com.cmd, "put")) {
    if (com.argc < 3)
      printf("ERROR(input: 'put' - too few arguments)\n");
    else if (com.argc > 3)
      printf("ERROR(input: 'put' - too many arguments)\n");
    else
      vfs_put(com.argv[1], com.argv[2]);
  } else if (!strcmp(com.cmd, "cat")) {
    if (com.argc < 2)
      printf("ERROR(input: 'cat' - too few arguments)\n");
    else if (com.argc > 2)
      printf("ERROR(input: 'cat' - too many arguments)\n");
    else
      vfs_cat(com.argv[1]);
  } else if (!strcmp(com.cmd, "cp")) {
    if (com.argc < 3)
      printf("ERROR(input: 'cp' - too few arguments)\n");
    else if (com.argc > 3)
      printf("ERROR(input: 'cp' - too many arguments)\n");
    else
      vfs_cp(com.argv[1], com.argv[2]);
  } else if (!strcmp(com.cmd, "mv")) {
    if (com.argc < 3)
      printf("ERROR(input: 'mv' - too few arguments)\n");
    else if (com.argc > 3)
      printf("ERROR(input: 'mv' - too many arguments)\n");
    else
      vfs_mv(com.argv[1], com.argv[2]);
  } else if (!strcmp(com.cmd, "rm")) {
    if (com.argc < 2)
      printf("ERROR(input: 'rm' - too few arguments)\n");
    else if (com.argc > 2)
      printf("ERROR(input: 'rm' - too many arguments)\n");
    else
      vfs_rm(com.argv[1]);
  } else
    printf("ERROR(input: command not found)\n");
  return;
}

int get_free_block(){
  if(sb->free_block==0){
    return -1;
  }
  int bloco = sb->free_block;
  sb->free_block=fat[bloco];
  fat[bloco]=-1;
  sb->n_free_blocks--;
  return bloco;
}





/* qsort C-string comparison function */ 
int cmp(const void *a, const void *b) 
{ 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
  /* strcmp functions works exactly as expected from
  comparison function */ 
} 

// ls - lista o conteúdo do diretório actual
void vfs_ls(void) {
  char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun", "Jul","Aug","Sep","Oct","Nov","Dec"};
  dir_entry *dir = (dir_entry *) BLOCK (current_dir);
  int n_entradas = dir[0].size;
  char *names[n_entradas];
  int i=0;
  for(i=0;i<n_entradas;i++){
    names[i]=dir[i].name;
  }
  qsort(names, n_entradas, sizeof(char *), cmp);
  int j=0;
  for(j=0;j<n_entradas;j++){
    for(i = 0;i<n_entradas;i++){
      if(strcmp(names[j],dir[i].name)==0){
        if(dir[i].type==TYPE_DIR){
          printf("%s\t%d-%s-%d\tDIR\n",dir[i].name,dir[i].day, months[dir[i].month-1], dir[i].year+1900); // imprimir directorios
          }
        else{
          printf("%s\t%d-%s-%d\t%d\n", dir[i].name,dir[i].day,months[dir[i].month-1],dir[i].year+1900,dir[i].size); // imprimir ficheiros. 
          }
        }
    }
    }
  return;

}


// mkdir dir - cria um subdiretório com nome dir no diretório actual
void vfs_mkdir(char *nome_dir) {
   dir_entry *dir = (dir_entry *) BLOCK (current_dir);
   int n_entradas = dir[0].size;
   //int bloco = get_free_block();
   int i=0;
   for(i=0;i<n_entradas;i++){
     if(strcmp(dir[i].name, nome_dir)==0){
       perror("same name\n");
       return ;
     }
   }

   int bloco=get_free_block();
   
    // o número de entradas no diretório (inicialmente 2) fica guardado no campo size da entrada "."
   init_dir_entry(&dir[n_entradas], TYPE_DIR, nome_dir, 0, bloco);
   //init_dir_entry(&dir[1], TYPE_DIR, "..", 0, parent_block);
   dir[0].size++;
   init_dir_block(bloco,current_dir);
   
    return;
}


// cd dir - move o diretório actual para dir
void vfs_cd(char *nome_dir) {
  dir_entry * aux = (dir_entry*) BLOCK  (current_dir);
  int n_entradas = aux[0].size;
  int i;
  for(i=0; i<n_entradas;i++){
    if(strcmp(aux[i].name,nome_dir)==0){
      current_dir = aux[i].first_block;
      return;
    }
  }
// Caso falhe a pesquisa do directory
printf("ERROR(cd: cannot move to directory `%s\' - entry does not exist)\n\t", nome_dir);
  return;

}

void vfs_pwd_aux(dir_entry * entry){
  int b;           //ciclo para blocos da fat
  int i;           //ciclo para as n_entradas
  dir_entry *aux;  //usar para ".." de entry e encontrar o nome de entry posteriormente
  
  /* se o directorio é a raiz --> regressa (termina recursão)*/
  if( entry->first_block == sb->root_block ){
     return;
  }
  aux = (dir_entry *)BLOCK( entry->first_block );
  vfs_pwd_aux(&aux[1]); // entrada ".." de `entry', anda para tras

  //tentar retirar o nome do proximo dir
  for( b = aux[1].first_block; b != -1; b = fat[b] ) {
     aux = (dir_entry *)BLOCK( b );
     int n_entradas=aux[0].size;
     for( i = 0; i < n_entradas; i++){
        if( aux[i].first_block == entry->first_block ){
           printf("/%s", aux[i].name);
           return;
        }
     }
  }

}



// pwd - escreve o caminho absoluto do diretório actual
void vfs_pwd(void) {
  dir_entry * entry = (dir_entry*)BLOCK(current_dir);

  if(entry->first_block == sb->root_block){ //estou no directorio raiz
    printf("/");
  }

  else{
    vfs_pwd_aux(entry); // sigo recursivamente ate a raiz
  }

  printf("\n");


  return;
}

void put_free_block(int nome){
  fat[nome]=sb->free_block;
  sb->free_block=nome;
  sb->n_free_blocks++;
  return;
}


// rmdir dir - remove o subdiretório dir (se vazio) do diretório actual
void vfs_rmdir(char *nome_dir) {
  dir_entry * entry = (dir_entry *) BLOCK(current_dir);
  int n_entradas=entry[0].size;
  //char names[n_entradas];
  int i=0;

  if( (strcmp(nome_dir, ".") == 0) || (strcmp(nome_dir, "..") == 0) ) {
    printf("ERROR(rmdir: cannot remove directory `%s\' - invalid directory)\n", nome_dir);
    return;
  }
  /*
    for(i=0;i<n_entradas;i++){
    names[i]=entry[i].name;
    }
  */
  for(i=0;i<n_entradas;i++){

    if(strcmp(nome_dir,entry[i].name)==0 && entry[i].type==TYPE_DIR){
	dir_entry *aux = (dir_entry *)BLOCK(entry[i].first_block);
	if (aux[0].size != 2){       //tem conteudo para alem dos directorios iniciais "." e ".."
	  printf("File not empty");
	  return;
	}
	else{                                             //podemos remover
	  put_free_block(entry[i].first_block);             //pomos bloco disponivel
	  entry[0].size--;                                  //diminuimos o tamanho do dir actual
	  memcpy(&entry[i],&entry[n_entradas-1],sizeof(dir_entry));
	}
      }
    /* else{
	printf("Directorio não existe ou e um ficheiro....\n");
	}*/
  }
  
    return;
  }


// get fich1 fich2 - copia um ficheiro normal UNIX fich1 para um ficheiro no nosso sistema fich2
void vfs_get(char *nome_orig, char *nome_dest) {
  return;
}


// put fich1 fich2 - copia um ficheiro do nosso sistema fich1 para um ficheiro normal UNIX fich2
void vfs_put(char *nome_orig, char *nome_dest) {
  return;
}


// cat fich - escreve para o ecrã o conteúdo do ficheiro fich
void vfs_cat(char *nome_fich) {
  return;
}


// cp fich1 fich2 - copia o ficheiro fich1 para fich2
// cp fich dir - copia o ficheiro fich para o subdiretório dir
void vfs_cp(char *nome_orig, char *nome_dest) {
  return;
}


// mv fich1 fich2 - move o ficheiro fich1 para fich2
// mv fich dir - move o ficheiro fich para o subdiretório dir
void vfs_mv(char *nome_orig, char *nome_dest) {
  return;
}


// rm fich - remove o ficheiro fich
void vfs_rm(char *nome_fich) {
  return;
}
