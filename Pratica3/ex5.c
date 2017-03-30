#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>

int main(){

  pid_t p;
  
  p=fork();

  if(p==0){
    printf("Filho PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
    execlp("ls","ls","-l",NULL);
    exit(1);
  }
  else{
    printf("Pai PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
    exit(1);
  }


 return 0;

}
