#include <unistd.h>
#include <stdio.h>

int main(){
  pid_t p;

  if((p=fork())==0){
    //filho
    printf("Filho PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
  }
  else{
    //pai
    printf("Pai PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
  }
  return 0;
}
