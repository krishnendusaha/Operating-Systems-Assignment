#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main ()
{
   int n, mypid, parpid;

   printf("Parent: n = "); scanf("%d", &n);
   printf("going to fork\n");
   int k;
   /* Child creation */
   if (k=fork()) { /* Parent process */
      sleep(5);
      mypid = getpid();
      parpid = getppid();
      printf("k %d Parent: PID = %u, PPID = %u...\n",k, mypid, parpid);
      exit(0);
   } else {      /* Child process */
      mypid = getpid();
      parpid = getppid();
      printf("k %d Child : PID = %u, PPID = %u...\n",k, mypid, parpid);
      n = n * n;
   }

   printf("Process PID = %u: n = %d\n", mypid, n);

   exit(0);
}

