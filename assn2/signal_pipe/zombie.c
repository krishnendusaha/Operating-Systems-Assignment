#include<stdio.h>
#include<signal.h>
void abc();
int main()
{ 
        int pid;
	pid=fork();
	if(pid==0)
        { printf("%d ",getpid()); sleep(1);}
	else
	{
		signal(SIGCLD, abc);
		sleep(10);
		printf("Parent exiting\n");
	}
}
void abc()
{
	printf("child died\n");
}
