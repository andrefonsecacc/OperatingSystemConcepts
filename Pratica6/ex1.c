#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

void *doSomething(void *arg);

pthread_t tid[2];
sem_t sem,sem1;

int main(){
  //sem_t sem,sem1;
  //pthread_t tid[2];
  int n_threads=0,err;

  sem_init(&sem,0,0);
  sem_init(&sem1,0,1);
  while(n_threads<2){

    err= pthread_create(&(tid[n_threads]),NULL,&doSomething,NULL);
    if(err!=0){
      printf("cannot create thread\n");
    }
    n_threads++;
  }
  
  n_threads=0;
  while(n_threads<2){
     pthread_join(tid[n_threads],NULL);
     n_threads++;
  }
  sem_destroy(&sem);
  sem_destroy(&sem1);
  

}

void * doSomething(void *arg){
  pthread_t id= pthread_self();
  int j=1;
  while(j<21){
    if((j%2)!=0 && pthread_equal(id, tid[0])){
      sem_wait(&sem1);
      printf("Thread impar %d\n", j);
      sem_post(&sem);
      //j++;
    }
    else if((j%2)==0 && pthread_equal(id,tid[1])){
      sem_wait(&sem);
      printf("thread par %d\n",j);
      sem_post(&sem1);
      //j++;
    }
    j++;
  }
  

}
