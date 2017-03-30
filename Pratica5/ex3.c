#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#define SEM_NAME "/mysem"
#define SEM_FLAGS S_IRUSR | S_IWUSR
//global
int n=0;
sem_t sem;
sem_t sem1;



void add(){
  n++;
}

void vef(){
  if(n==20){
  sem_destroy(&sem);
  sem_destroy(&sem1);
  }
}

int main(){
  
  pid_t p;
  

  sem_init(&sem,0,1);
  sem_init(&sem1,0,0);

  p=fork();

  if(p==0){//filho
    
    if(sem_wait(&sem)==0){
      //n++;
      add();
      printf("Filho: %d\n",n);
      sem_post(&sem1);
      vef();
    }
    
  }
  else{//pai
    
    if(sem_wait(&sem1)==0){
      //n++;
      add();
      printf("Pai; %d\n",n);
      sem_post(&sem);
      vef();
    }
  }

  
}
