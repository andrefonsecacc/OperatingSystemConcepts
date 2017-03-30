#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc,char *argv[]){
  int n = atoi(argv[1]);
  //scanf("%d",&n); // ler n processos 
  int count;
  pid_t p;
  
  printf("PAI PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
  p=fork();
  
  for(count=0;count<n;count++){
    if(p==0){ // filho
      printf("FILHO PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
      break;
    }

     else{  
       p=fork();
    }
  }

  return 0;

}
