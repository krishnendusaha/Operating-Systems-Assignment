#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>


#define BUFSIZE 80

int fd_C[2] ,fd_D[2] ;
int num_c;
int num_d;
char line_C[BUFSIZE] , line_D[BUFSIZE] ,line_P[BUFSIZE];

void childSigHandler ( int sig )
{
   if (sig == SIGUSR1) {
      printf("+++ C -I have won the game...Bye!\n");
      sleep(1);
      exit(0);
   } else if (sig == SIGUSR2) {
     printf("+++ C -Congratulations D for winning...Bye!\n");
	 sleep(1);
     exit(0);
   }
   else if(sig == SIGIO)
   {

   	    num_c = rand() % 4;
		sprintf(line_C,"%d",num_c);
		printf("Child C sends %d\n",num_c);
		write(fd_C[1],line_C,BUFSIZE);
   	    
   }

 
   
}

void childSigHandler2(int sig)
{
	if(sig == SIGUSR1)
	{
		printf("+++ D -I have won the game...Bye!\n");
		sleep(1);
        exit(0);
	}
	else if(sig == SIGUSR2)
	{
		printf("+++ D -Congratulations c for winning...Bye!\n");
		sleep(1);
        exit(0);
	}
	else if(sig == SIGIO)
	{
		num_d = rand() % 4;
		sprintf(line_D,"%d",num_d);
		printf("Child D sends %d\n",num_d);
		write(fd_D[1],line_D,BUFSIZE);
	}

  

}

	
int main()
{

	
	
	int pid,ppid;
	int pid_C,pid_D;
	
	pipe(fd_C);
	pipe(fd_D);
	int flag ; 
	
	int score_C =0,score_D =0;

	// flag = 0 stands for MIN
	// flag = 1 stands for MAX

	if(( pid_C = fork() ) ==  0 ) //Child Process C.
	{
		srand((long int)&pid_C);
		signal(SIGUSR1, childSigHandler);           /* Register SIGUSR1 handler */
                signal(SIGUSR2, childSigHandler); 
                signal(SIGIO, childSigHandler);             /* Register SIGUSR2 handler */	
	  	close(fd_C[0]);
		while(1)
		{
	         sleep(5);
		}
		
	}
	else if((pid_D = fork()) == 0) //Child Process D
	{	
		srand((long int)&pid_D);
		signal(SIGUSR1, childSigHandler2);           /* Register SIGUSR1 handler */
                signal(SIGUSR2, childSigHandler2);           /* Register SIGUSR2 handler */
		signal(SIGIO, childSigHandler2);
		close(fd_D[0]);
		while(1)
                {
		  sleep(5);
                }
	}
	else 						   //Parent Process 
	{
		srand(time(NULL));
		close(fd_C[1]);
		close(fd_D[1]);
		while((score_C < 10) && (score_D < 10)){
		flag = rand() % 2 ; //returns either 0 or 1.
		printf("Parent is trying to read C's pipe\n");
		kill(pid_C,SIGIO);
		read(fd_C[0],line_P,BUFSIZE);
		sscanf(line_P,"%d",&num_c);
		printf("Parent has read %d from C\n",num_c);
		printf("Parent is trying to read D's pipe\n");
		kill(pid_D,SIGIO);
		read(fd_D[0],line_P,BUFSIZE);
		sscanf(line_P,"%d",&num_d);
		printf("Parent has read %d from D\n",num_d);
		if(flag == 0) // MIN
		{
			printf("Parent has chosen MIN in this round \n");
			if( num_c < num_d) 
				score_C ++ ;
			else if(num_c > num_d)
				score_D ++ ;
			else
				printf("Round Ignored\n");

		}
		else  //MAX
		{
			printf("Parent has chosen MAX in this round \n");
			if( num_c > num_d) 
				score_C ++ ;
			else if(num_c < num_d)
				score_D ++ ;
			else
				printf("Round Ignored\n");


		}
		sleep(1);
		printf("score_C =%d \nscore_D = %d \n",score_C,score_D);
	}

	if(score_C == 10)
	{
		kill(pid_C,SIGUSR1);
		kill(pid_D,SIGUSR2);
	}
	else
	{
		kill(pid_C,SIGUSR2);
		kill(pid_D,SIGUSR1);
	}

	wait(NULL);
	}
	printf("+++Parent - Nice Game Kids!\n");
	return 0;
}
