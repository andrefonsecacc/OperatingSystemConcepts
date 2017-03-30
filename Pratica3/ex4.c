#include <unistd.h>
#include <stdio.h>

int main(){
  int state=0;
  int n,n_filho=1;
  scanf("%d",&n);
  int count;
  pid_t p;
  

  printf("PAI PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
  //p=fork();
  while(state==0){
    p=fork();
    if(p==0){
        printf("Filho PID %d PPID %d UID %d\n",getpid(),getppid(),getuid());
	n_filho++;
	if(n_filho==(n+1)){
	  state=1;
	}
    }
    else{
      break;
	}
  }

  return 0;

}
