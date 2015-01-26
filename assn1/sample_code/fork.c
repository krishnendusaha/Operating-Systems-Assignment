#include  <stdio.h>
#include  <sys/types.h>

#define   MAX_COUNT  200

void  ChildProcess(void);                /* child process prototype  */
void  ParentProcess(void);               /* parent process prototype */

void  main(void)
{
     pid_t  pid;

     pid = fork();
     if (pid == 0) 
         { ChildProcess(); printf(" child ends\n");}
     else 
         { int k;  printf("\nparent goes to sleep\n"); k=usleep(100);printf("\nparent awake\n");  ParentProcess(); printf("k: %d parent ends\n",k);}
}

void  ChildProcess(void)
{
     int   i;
     printf("*******%d ******* \n",gethostname());
     for (i = 1; i <= MAX_COUNT; i++)
          printf("   This line is from child, value = %d\n", i);
     printf("   *** Child process is done ***\n");
}

void  ParentProcess(void)
{
     int   i;

     for (i = 1; i <= MAX_COUNT; i++)
          printf("This line is from parent, value = %d\n", i);
     printf("*** Parent is done ***\n");
}