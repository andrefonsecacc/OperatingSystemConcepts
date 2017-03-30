#include<signal.h>
#include<unistd.h>
#include<stdio.h>

#define SIG_ERR (void(*)()) -1
#define SIG_DFL (void(*)()) 0
#define SIG_IGN (void(*)()) 1

int recebi_sinal=0;

void sig_usr(int signo){
  //if(signo == SIGUSR1) printf("received SIGUSR1\n");
  recebi_sinal =1;
}
void sig_stp(int signo){
  if(signo == SIGTSTP) printf("received SIGTSTP %d\n",getpid());
}

void sig_int(int signo){
  if(signo == SIGINT) printf("received SIGINT %d\n",getpid());
}
void sig_term(int signo){
  if(signo == SIGTERM) printf("received SIGTERM %d\n",getpid());
}

void sig_kill(int signo){
  if(signo==SIGKILL) printf("received SIGKILL %d \n",getpid());
}


int main(){

  pid_t p;
  signal(SIGUSR1,sig_usr);
  p=fork();
  
  //if(signal(SIGUSR1,sig_usr)==SIG_ERR){/*signal error*/}
  if(signal(SIGTSTP,sig_stp)==SIG_ERR){/*signal error*/}
  if(signal(SIGINT,sig_int)==SIG_ERR){/*signal error*/}
  if(signal(SIGTERM,sig_term)==SIG_ERR){/*signal error*/}
  if(signal(SIGKILL,sig_kill)==SIG_ERR){/*signal error*/}

  // while(1){
    if(p==0){//filho
      //if(signal(SIGUSR2,sig_usr)==SIG_ERR){/*signal error*/}
      // pause();
      printf("FILHO\n");
      kill(getppid(),SIGUSR1);
    }
    else{//pai
      // if(signal(SIGUSR1,sig_usr)==SIG_ERR){/*signal error*/}
      // pause();
      while(recebi_sinal==0);
      printf("PAI\n");
      //signal(SIGUSR2,sig_usr);
    }

    // }



  
}
