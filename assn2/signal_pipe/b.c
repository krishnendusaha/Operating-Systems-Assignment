#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
void abc();
void def();
int pid;
int main()
{
     pid=fork();
	if(pid==0)
	{
		signal(SIGUSR2, abc);
		sleep(1);
		printf("Hello parent!");
		kill(getppid(),SIGUSR1);
		sleep(4);				
	}
	else
	{
		signal(SIGUSR1,def);
		sleep(5);
	}
}
void abc()
{	sleep(2);
	printf("Bye  Parent ");
}

void def()
{
	printf("Hello child");
	kill(pid,SIGUSR2);
}
