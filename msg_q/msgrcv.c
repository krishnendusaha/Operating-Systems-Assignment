#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include  <sys/shm.h>
#include <sys/msg.h>

struct message
{
        long mtype;
        char mtext[15];
};

int main()
{	int msgid,len=20;
	key_t key;
    struct message buff;

    key=131;
    msgid=msgget(key,IPC_CREAT|0666);
    printf("\nq=%d",msgid);


    if(msgrcv(msgid,&buff,len,0,0)==-1) 
    {   
        perror("msgrv failed\n");
        exit(1);
	}   
    printf("\nmsg received %s",buff.mtext); 
  return 0;
}

