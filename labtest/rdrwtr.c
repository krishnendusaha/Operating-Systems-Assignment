#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>

#define  VAR_MAX  100
#define P(s) semop(s, &pop, 1) 
#define V(s) semop(s, &vop, 1)  

int main()
{
	
  int pid1,pid2,status;
  FILE * buf;
 
  int shmid0=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT); 
  int shmid1=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
  int shmid2=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT);
  
  int * var= (int *) shmat(shmid0, 0, 0);
  int * exit_flg=(int *) shmat(shmid1, 0, 0);
  int *r_count=(int *) shmat(shmid2, 0, 0); 
  int write_sem;
  struct sembuf pop, vop ;
  write_sem = semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(write_sem,0,SETVAL,1);// set the mutex to 1.
  
  int mutex=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(mutex,0,SETVAL,1);

  int mutex1=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(mutex1,0,SETVAL,1);

  pop.sem_num = vop.sem_num = 0;
  pop.sem_flg = vop.sem_flg = 0;
  pop.sem_op = -1 ; vop.sem_op = 1 ;
  

  if((pid1=fork())==0)
  {    
    usleep(10);
    while(exit_flg[0]==0)
    {
      P(mutex);
      r_count[0]++; 
      if(r_count[0]==1) {   P(write_sem); printf("First Reader enters \n "); }  
    	V(mutex);   
      buf=fopen("buffer.txt","r");
    	fscanf(buf,"%d",&(var[0]));
      printf("Reader (%d) reads the value %d\n",getpid(),var[0]);
      fclose(buf);
      P(mutex);
      r_count[0]--;
      if(r_count[0]==0) { printf("Last Reader leaves \n"); V(write_sem);}
      V(mutex);
      usleep(5);
    }
 
    exit(0);
  }
  else if(pid2=fork()==0)
  {
     usleep(10);
      while(exit_flg[0]==0)
    {
      P(mutex);
      r_count[0]++; 
      if(r_count[0]==1) {    P(write_sem); printf("First Reader enters \n "); }  
      V(mutex);   
      buf=fopen("buffer.txt","r");
      fscanf(buf,"%d",&(var[0]) );
      printf("Reader (%d) reads the value %d\n",getpid(),var[0]);
      fclose(buf);
      P(mutex);
      r_count[0]--;
      if(r_count[0]==0) { printf("Last Reader leaves \n"); V(write_sem);}
      V(mutex);
      usleep(5);
    }
    exit(0);
  }
  else{
      while(var[0]<=VAR_MAX)
      {
        P(write_sem);
        printf("writing starts\n");
    	  var[0]++;
        buf=fopen("buffer.txt","w");
    	  fprintf(buf, "%d ",var[0] );
        printf("Writer (%d) writes the value %d\n",getpid(),var[0]);
        fclose(buf);
        printf("writing ends\n");
        V(write_sem);
        usleep(10);
      }   
       
      exit_flg[0]=1;

      waitpid(pid1,&status,0);
      waitpid(pid2,&status,0);
  }


}