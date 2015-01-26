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
#define THINKING 0
#define HUNGRY   1
#define EATING   2
#define LEFT    (i+N-1)%N
#define RIGHT   (i+1)%N
#define MEAN_THINK_TIME		100	/* avg think time in milliseconds */
#define MEAN_EAT_TIME		75	/* avg eat time in milliseconds */



/*
 * Static variable to record the number of times the generator has been
 * initialized.
 */

#define P(s,i) semop(s, &(pop[i]), 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define V(s,i) semop(s, &(vop[i]), 1)  /* vop is the structure we pass for doing
				   the V(s) operation */

#define PP(s) semop(s, &mutex_pop, 1)  /* pop is the structure we pass for doing
				   the P(s) operation */
#define VV(s) semop(s, &mutex_vop, 1)  /* vop is the structure we pass for doing
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
	start = times( &buf );		/* First time */
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
    return;
}

void eat(int i)
{
	printf("Philosopher %d EATING\n",i );
  usleep( 1000L * random_int( MEAN_EAT_TIME ) );/* microseconds */
  printf("Philosopher %d has finished EATING\n", i);
    return;	
}


void test(int i,int semid,int*A,struct sembuf *pop,struct sembuf *vop)
{

	if((A[i] == HUNGRY * 1) && (A[LEFT] !=  EATING * 1 )&& (A[RIGHT] != EATING * 1))
	{
		A[i] = EATING;
		V(semid,i);
	}
} 

void take_forks(int i,int semid,int* A,int mutexid,struct sembuf *pop,struct sembuf *vop,struct sembuf mutex_pop,struct sembuf mutex_vop)
{
    
	//printf("Philosopher %d entered here \n",i );
	PP(mutexid);
	printf("philosopher %d HUNGRY\n", i);
	A[i] = HUNGRY;
	test(i,semid,A,pop,vop);
	VV(mutexid);
	P(semid,i);	


}


void put_forks(int i,int semid,int* A,int mutexid,struct sembuf *pop,struct sembuf *vop,struct sembuf mutex_pop,struct sembuf mutex_vop)
{

	PP(mutexid);
	A[i] = THINKING;
	printf("Philosopher %d THINKING\n",i );
	test(LEFT,semid,A,pop,vop);
	test(RIGHT,semid,A,pop,vop);
	VV(mutexid);

}

void philosopher(int i,int duration,int semid,int* A,int mutexid,struct sembuf *pop,struct sembuf *vop,struct sembuf mutex_pop,struct sembuf mutex_vop)
{

  // printf("Philosopher %d started \n",i );	
   int  start_time = msecond();

   while(msecond() - start_time < duration * 1000 )
   {
   	  think(i);
   	  take_forks(i,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
   	  eat(i);
   	  put_forks(i,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
   }

}


int main()
{
   int pid[5];
   int semid,i;
   int duration;
   struct sembuf pop[5], vop[5] ;

   //One semaphore with 5 subsemaphores
   int shmid = shmget(IPC_PRIVATE, 5*sizeof(int), 0777|IPC_CREAT);
   int *A=(int *) shmat(shmid, 0, 0); 
   
   int mutexid = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
   struct sembuf mutex_pop,mutex_vop;
   mutex_pop.sem_num = mutex_vop.sem_num = 0;
   mutex_pop.sem_flg = mutex_vop.sem_flg = 0;
   mutex_pop.sem_op = -1;
   mutex_vop.sem_op = 1; 

   int status;

   //initialize with thinking
   for(i=0;i<5;i++)
   {
   	A[i] = 0;
   }

//this pop and vop array is used to access different semaphores.
  pop[0].sem_num = vop[0].sem_num = 0;
	pop[0].sem_flg = vop[0].sem_flg = 0;
	pop[0].sem_op = -1 ; vop[0].sem_op = 1 ;
    

  pop[1].sem_num = vop[1].sem_num = 1;
	pop[1].sem_flg = vop[1].sem_flg = 0;
	pop[1].sem_op = -1 ; vop[1].sem_op = 1 ;


  pop[2].sem_num = vop[2].sem_num = 2;
	pop[2].sem_flg = vop[2].sem_flg = 0;
	pop[2].sem_op = -1 ; vop[2].sem_op = 1 ;

    
  pop[3].sem_num = vop[3].sem_num = 3;
	pop[3].sem_flg = vop[3].sem_flg = 0;
	pop[3].sem_op = -1 ; vop[3].sem_op = 1 ;


  pop[4].sem_num = vop[4].sem_num = 4;
	pop[4].sem_flg = vop[4].sem_flg = 0;
	pop[4].sem_op = -1 ; vop[4].sem_op = 1 ;

	



   printf("Enter duration  of running : \n");
   scanf("%d",&duration);

 //   for(i=0;i<5;i++){
 //   semid[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
 //   semctl(semid[i], 0, SETVAL, 0);
	// }

  //initialize with 0. 
   
   semid = semget(IPC_PRIVATE, 5, 0777|IPC_CREAT);
   semctl(semid,0,SETALL,0);

   //mutex with 1
   semctl(mutexid, 0, SETVAL,1);

   if((pid[0]=fork())==0)
   {
       philosopher(0,duration,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
       printf("Philosopher 0 left the table\n");
       //for(i=0;i<5;i++)
       //{
     	//printf("%d\n",A[i]);
       //}
      exit(1);
   }
   else if((pid[1]=fork())==0)
   {
       philosopher(1,duration,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
       printf("Philosopher 1 left the table\n");
       //for(i=0;i<5;i++)
       //{
        //	printf("%d\n",A[i]);
      // }
        exit(1);
   }
   else if((pid[2]=fork())==0)
   {
   	   philosopher(2,duration,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
   	   printf("Philosopher 2 left the table\n"); 
   	//for(i=0;i<5;i++)
   //{
   	//printf("%d\n",A[i]);
  // }
   	   exit(1);
   }
   else if((pid[3]=fork())==0)
   {
   	   philosopher(3,duration,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
       printf("Philosopher 3 left the table\n"); 
   // 	for(i=0;i<5;i++)
   //{
   //	printf("%d\n",A[i]);
   //}  	
       
       exit(1);  	   
   }
   else if((pid[4]=fork())==0)
   {
   	   philosopher(4,duration,semid,A,mutexid,pop,vop,mutex_pop,mutex_vop);
       printf("Philosopher 4 left the table\n");   	   
    //for(i=0;i<5;i++)
   //{
   	//printf("%d\n",A[i]);
  // }
       exit(1);
   }
   else{
      
        waitpid(pid[0], &status, 0); 
        waitpid(pid[1], &status, 0);                         //wait for  all the child  processes to end  
        waitpid(pid[2], &status, 0);
        waitpid(pid[3], &status, 0);
        waitpid(pid[4], &status, 0);
       // wait(NULL);
        printf("Dining Done completely\n");
                 
   }


}



