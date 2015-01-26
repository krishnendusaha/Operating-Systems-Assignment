#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include  <sys/shm.h>
int main()
{
	int msgid,len;
	key_t key;
	key=131;
	msgid=msgget(key,IPC_CREAT|0666);
	printf("\nq=%d\n",msgid);
}
