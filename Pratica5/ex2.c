#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#define SEM_NAME "/mysem"
#define SEM_FLAGS S_IRUSR | S_IWUSR

sem_t sem; //semaphore pointer


int main(){
 
  //sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_FLAGS, 1);
  sem_init(&sem,0,1);
  pid_t p ;

  p=fork();


  if(p==0){//filho
    while(1){
      printf("FILHO\n");
      sem_post(&sem);
      sem_wait(&sem);
    }
  }
  else{//pai
    while(1){
    sem_wait(&sem);
    printf("PAI\n");
    //sem_destroy(&sem);
    sem_post(&sem);
    }
  }

  
  //sem_close(sem);
  //sem_unlink(SEM_NAME);

  
}
