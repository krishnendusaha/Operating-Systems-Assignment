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


#define N 5
#define LEFT     i
#define RIGHT   (i+1)%N
#define MEAN_THINK_TIME   1000  /* avg think time in milliseconds */
#define MEAN_EAT_TIME   750 /* avg eat time in milliseconds */

/*
 * Static variable to record the number of times the generator has been
 * initialized.
 */

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing
           the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing
           the V(s) operation */


static int initialized = 0;

/*--------------------------------------------------------------------------*/

void init_random_int( int seed )
/*
 * Initializes the random number generator.  If seed is negative then the
 * system clock is used to initialize the generator.  A count is also kept
 * of the number of times this routine has been called.
 */
{
    if ( seed < 0 ) {
  seed = (int) time( (time_t *) 0 );
    }
    srandom( seed );
    initialized++;
}

/*--------------------------------------------------------------------------*/

int random_int( int mean )
/*
 * Computes a random integer from an exponetial distribution with a
 * specified mean.  If this routine is called and the generator has not
 * yet been initialized, it initializes it using the system clock.
 */
{
    double u;
    int value;
    
    if ( !initialized ) {
  init_random_int( -1 );
    }

    /*
     * Random number from uniform distibution on [0,1)
     */
    u = (double) ( ( random() & RAND_MAX ) / ( 1.0 + RAND_MAX ) );

    /*
     * Find number from exponentially distribution with specified mean
     * and round to an integer.
     */
    value = (int) ( 0.5 - mean * log( u ) );

    if ( value == 0 ) {
  value = 1;
    } else if ( value > 5 * mean ) {
  value = 5 * mean;
    }

    return value;
}

/*--------------------------------------------------------------------------*/


int msecond( void )
/*
 * The first time msecond() is called it returns 0.  For all subsequent
 * calls it returns the number of milliseconds that have elapsed since
 * the first call.
 * 
 * The times() system call is used to obtain the number of clock cycles
 * that have elapsed since some point in the past (usually system boot
 * time).  This number is then divide by HZ which is a system-dependent
 * constant from <sys/params.h> that gives the number of clock cycles per
 * second.  Finally, we multiply by 1000 to convert to milliseconds.  The
 * upshot of all this is that msecond() does not return values with
 * millisecond accuracy.  On Linux systems and the SGI O2 the value of HZ
 * is 100 so that the resolution of this function is about 10 milliseconds.
 */
{
    static clock_t start = (clock_t) 0;
    struct tms buf;

    if ( start == (clock_t) 0 )
    {
  start = times( &buf );    /* First time */
  return 0;
    } else {
  return (int) ( ( 1000 / HZ ) * ( times( &buf ) - start ) );
    }
}



void think(int i)
{
    printf("Philosopher %d THINKING\n",i );
    usleep( 1000L * random_int( MEAN_THINK_TIME ) );/* microseconds */
    printf("Philosopher %d has finished THINKING\n",i );
    //return;
}

void eat(int i,int* A1)
{
    printf("Philosopher %d EATING\n",i );
    usleep( 1000L * random_int( MEAN_EAT_TIME ) );/* microseconds */
    A1[0]++;
    printf("Philosopher %d has finished EATING\n", i);
    //return; 
}


void philosopher(int i,int duration,int *semid,int* A,struct sembuf pop,struct sembuf vop,int mutexid,int* A1)
{

   printf("Philosopher %d started \n",i );  
   int  start_time = msecond();

   while(msecond() - start_time < duration * 1000 )
   {
      think(i);
     // printf("about to take fork left - %d\n",i);
      P(semid[i]);
      printf("Philosopher %d has taken fork %d (left) \n",i,i );
      //Set the resource graph value within mutex as it is shared memory
      P(mutexid);
      A[LEFT] = i;
      V(mutexid);

      usleep( 1000L * random_int( MEAN_THINK_TIME ) );/* microseconds */
      

      P(semid[(i+1) % N]);
      P(mutexid);
      
      if(A[LEFT] == -1) // if the left fork isn't free and it got the right fork,drop the right fork as well. 
      {
       V(semid[(i+1)%N]);
       continue;
      }

      A[RIGHT] = i;
      V(mutexid);
      
      printf("Philosopher %d has taken fork %d (right)\n",i, (i+1)%N );
      
      eat(i,A1);
      
      V(semid[i]);
      printf("Philosopher %d releases fork %d (left)\n",i, i);
      
      P(mutexid);
      A[LEFT] = -1;
      V(mutexid);
      
      V(semid[(i+1)%N]);
      printf("Philosopher %d releases fork %d (right)\n",i, (i+1)%N);
      
      P(mutexid);
      A[RIGHT] = -1;
      V(mutexid);
   }
}

int main()
{
   int pid[5];
   int semid[5],i,status;
   
   int duration;
   struct sembuf pop, vop ;
   int mutexid;
   int number_of_deadlocks = 0;

   //Resource graph here is implemented using an ADJACENCY MATRIXof shared memory.
   //It has the details about which fork is being used by which philosopher.

   int shmid = shmget(IPC_PRIVATE, 5*sizeof(int), 0777|IPC_CREAT);
   int *A=(int *) shmat(shmid, 0, 0); 

   //Shared memory to count the number of meals
   int shmid1 = shmget(IPC_PRIVATE, 1*sizeof(int), 0777|IPC_CREAT);
   int *A1=(int *) shmat(shmid1, 0, 0); 

   for(i=0;i<5;i++)
   {
    A[i] = -1; // -1 here means that the fork is empty.No one is currently using it.
   }

  A1[0] = 0;
  // 5 individual semaphores for 5 forks.
  for(i=0;i<5;i++){
  semid[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
  semctl(semid[i], 0, SETVAL, 1); // set them to 1.
  }   

  //mutex for the shared memory.
  mutexid = semget(IPC_PRIVATE,1,0777|IPC_CREAT);
  semctl(mutexid,0,SETVAL,1);// set the mutex to 1.

  //sembuf structure

  pop.sem_num = vop.sem_num = 0;
  pop.sem_flg = vop.sem_flg = 0;
  pop.sem_op = -1 ; vop.sem_op = 1 ;
    


  printf("Enter duration  of running : \n");
  scanf("%d",&duration); 
  
  if((pid[0]=fork())==0)
    {
       philosopher(0,duration,semid,A,pop,vop,mutexid,A1);
       printf("Philosopher 0 has left table\n");
       //for(i=0;i<5;i++)
       //{
      //printf("%d\n",A[i]);
      // }
      exit(1);
    }
    else if((pid[1]=fork())==0)
    {
       philosopher(1,duration,semid,A,pop,vop,mutexid,A1);
       printf("Philosopher 1 has left table\n");
       //for(i=0;i<5;i++)
      // {
        //  printf("%d\n",A[i]);
      // }
        exit(1);
    }
   else if((pid[2]=fork())==0)
   {
       philosopher(2,duration,semid,A,pop,vop,mutexid,A1);
       printf("Philosopher 2 has left table\n"); 
    //for(i=0;i<5;i++)
   //{
    //printf("%d\n",A[i]);
   //}
       exit(1);
   }
   else if((pid[3]=fork())==0)
   {
       philosopher(3,duration,semid,A,pop,vop,mutexid,A1);
       printf("Philosopher 3 has left table\n"); 
    //  for(i=0;i<5;i++)
   //{
   // printf("%d\n",A[i]);
   //}    
       
       exit(1);      
   }
   else if((pid[4]=fork())==0)
   {
       philosopher(4,duration,semid,A,pop,vop,mutexid,A1);
       printf("Philosopher 4 has left table\n");       
   // for(i=0;i<5;i++)
   //{
   // printf("%d\n",A[i]);
   //}
       exit(1);
   }
   else{
            int  parent_time = msecond();
            while(msecond() - parent_time < duration * 1000 ){
            usleep( 1000L * random_int( MEAN_THINK_TIME ) );
            //for(i=0;i<5;i++)
            //{
            //printf("Parent printing - %d\n",A[i]);
           // }
            P(mutexid);
            if(A[0] == 0 && A[1] == 1 && A[2] == 2 && A[3] == 3 && A[4] == 4 )
            {
              // Deadlock detected 
              printf("Parent detects deadlock,going to initiate recovery\n");
              number_of_deadlocks++;
              i =  rand() % 5;
              printf("Parent preempts Philosopher %d\n",i);
              V(semid[i]); 
              printf("Philosopher %d releases fork %d(left)\n",i,i );
              A[i] = -1;

            }
            V(mutexid);
            //These are enclosed within P and V (mutexid) because they are accessing shared memory.


       }

        waitpid(pid[0], &status, 0); 
        waitpid(pid[1], &status, 0);                         //wait for  all the child  processes to end  
        waitpid(pid[2], &status, 0);
        waitpid(pid[3], &status, 0);
        waitpid(pid[4], &status, 0);
       // wait(NULL);
        printf("Dining Done completely with %d meals and %d deadlocks\n",A1[0],number_of_deadlocks );
                 
   }

   

}