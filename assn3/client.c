#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#define  BUFFER_SIZE 256
#define  UP  11
#define  DN  12


struct message
{
        long mtype;
        char mtext[BUFFER_SIZE];
};




int main(int argc,int **argv)
{
	int UPmsqid,DNmsqid;
    struct message msgsend,msgrecv;
    char * input=(char*)malloc(sizeof(char)*256);
    
    UPmsqid=msgget(UP,IPC_CREAT|0666);
    DNmsqid=msgget(DN,IPC_CREAT|0666);
    while(1){
    printf("Insert message to send to server:");
    gets(input);
    printf("\n");
   // if(strcmp(input,"quit")==0){ return 0;  }
    strcpy(msgsend.mtext,input);
    msgsend.mtype=1;
   
    if(msgsnd(UPmsqid,(void*)&msgsend,BUFFER_SIZE,0)==-1)
	{
		printf("Client:message sending failed\n");
		exit(1);
	}
   
    
    if(msgrcv(DNmsqid,(void*)&msgrecv,BUFFER_SIZE,0,0)==-1) 
    {   
        perror("Client:message receive failed\n");
        exit(1);
	}   
    if(strcmp(msgrecv.mtext,"QUIT") == 0)
    {
        msgctl(UPmsqid,IPC_RMID,NULL);
         msgctl(DNmsqid,IPC_RMID,NULL); 
    }
    printf("Processed msg from server :%s\n",msgrecv.mtext );
   }
   return 0;
}