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

#define  VAR_MAX  10
#define P(s) semop(s, &pop, 1) 
#define V(s) semop(s, &vop, 1)  

int main()
{
	
  int pid1,pid2;
  FILE * buf;
  int r_count=0;
  int var=0;
  int shmid=shmget(IPC_PRIVATE, sizeof(int), 0777|IPC_CREAT); 
  int * exit_flg=(int *) shmat(shmid, 0, 0); 
  int write_sem,read_sem;
  struct sembuf pop, vop ;
  write_sem = semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(write_sem,0,SETVAL,1);// set the mutex to 1.
  read_sem = semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(read_sem,0,SETVAL,1);// set the mutex to 1.
  int mutex=semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(mutex,0,SETVAL,1);

  pop.sem_num = vop.sem_num = 0;
  pop.sem_flg = vop.sem_flg = 0;
  pop.sem_op = -1 ; vop.sem_op = 1 ;
  

  if((pid1=fork())==0)
  {    
    usleep(10);
    while(exit_flg[0]==0)
    {
      if(r_count==0) { printf("First Reader enters \n ");P(read_sem); }
      r_count++;
    	
      P(mutex); 
      buf=fopen("buffer.txt","r");
    	fscanf(buf,"%d",&var);
      printf("Reader (%d) reads the value %d\n",getpid(),var);
      fclose(buf);
      V(mutex);   
      
      r_count--;
      if(r_count==0) { printf("Last Reader leaves \n"); V(read_sem);}
      usleep(5);
    }

    exit(0);
  }
  else if(pid2=fork()==0)
  {
     usleep(10);
     while(exit_flg[0]==0)
     {
      if(r_count==0) {  printf("First Reader enters \n "); P(read_sem); }
       r_count++;
      P(mutex);
      buf=fopen("buffer.txt","r");
    	fscanf(buf,"%d",&var);
      printf("Reader (%d) reads the value %d\n",getpid(),var);   
      fclose(buf);
      V(mutex);
      
      if(r_count==0) {   printf("Last Reader leaves \n");   V(read_sem); }
      usleep(5);
     }
    exit(0);
  }
  else{
      while(var<=VAR_MAX)
      {
        P(read_sem);
      
    	  var++;
        P(mutex);
        buf=fopen("buffer.txt","w");
    	  fprintf(buf, "%d ",var );
        printf("Writer (%d) writes the value %d\n",getpid(),var);
        fclose(buf);
        V(mutex);
        V(read_sem);
        // usleep(10);
      }   
       
      exit_flg[0]=1;

  }


}