#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include  <sys/shm.h>
#include <sys/msg.h>
int main()
{

	int qid;
	struct msqid_ds qstat;
	qid=msgget((key_t)131,IPC_CREAT);
	if(qid==-1)
	{
		perror("msg failed\n");
		exit(1);
	}
	if(msgctl(qid,IPC_STAT,&qstat)<0)
	{
		perror("msgctl failed");
		exit(1);
	}
	printf("\n%d msg in q",(int)qstat.msg_qnum);
	printf("\nlast msg send by process %d",(int)qstat.msg_lspid);
	printf("\nlast msg receved by process %d",(int)qstat.msg_lrpid);
	printf("\ncurrent number of bytes on queue %d",(int)qstat.msg_cbytes);
	printf("\nmax number of bytes %d\n",(int)qstat.msg_qbytes);

	return 0;
}