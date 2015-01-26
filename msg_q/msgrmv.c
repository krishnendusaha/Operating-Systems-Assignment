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
	int qid=98304 ;
	if(msgctl(qid,IPC_RMID,NULL)<0)
	{
		perror("msgctl failed");
		exit(1);
	}
    return 0;
}
