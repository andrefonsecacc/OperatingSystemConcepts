#include <unistd.h>
#include <stdio.h>

int main(){
  int count;
  pid_t p;
  
  printf("PAI PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
  p=fork();
  
  for(count=0;count<2;count++){
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
