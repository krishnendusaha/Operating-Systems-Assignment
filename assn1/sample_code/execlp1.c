#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
main()
{
    int pid,j=10,fd;
    pid=fork();
    if(pid==0)
    {
        printf("\nI am the child\n");
        execlp("/bin/ls","ls",NULL);
        printf("\nStill I am the child\n");

    }
    else if (pid > 0)
    {
        printf("\n I am the parent\n");
        wait();
    } 
}