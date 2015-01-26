#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#define  UP  11
#define  DN  12
#define  BUFFER_SIZE 256

struct message
{
        long mtype;
        char mtext[BUFFER_SIZE];
};



int main()
{
	  int UPmsqid,DNmsqid,i;
 	  key_t key1,key2;
    struct message msg2server,msg2client;

    //get the message queue IDs with keys 11 & 12 two randomly chosen number  
    //these two queues are going to be used as UP and Down 
    UPmsqid=msgget(UP,IPC_CREAT|0666);
    DNmsqid=msgget(DN,IPC_CREAT|0666);
     

    while(1)
    {
       //get the message from queue send by a client
        if(msgrcv(UPmsqid,(void*)&msg2server,BUFFER_SIZE,0,0)==-1) 
        {   
          perror("msgrcv failed\n");
          exit(1);
	      } 
       //get the time at which the message is received   
       struct msqid_ds qstatus; 
       msgctl(UPmsqid,IPC_STAT,&qstatus);
       time_t  msgrcv=qstatus.msg_rtime; 
       char time_msg[BUFFER_SIZE];
       strftime(time_msg,BUFFER_SIZE,"Msg received at time:%c\n",localtime(&msgrcv));
       
       //print the time 
       fprintf(stdout, "%s\n",time_msg );

       char c;

       //Invert the case of the msg string
     // if(strcmp(msg2server.mtext,"quit")==0){if( msgctl(UPmsqid,IPC_RMID,NULL)==-1) {printf("error\n");} if(msgctl(DNmsqid,IPC_RMID,NULL)==-1) {printf("errror\n");}    return 0;} 
       for(i=0;i<BUFFER_SIZE;i++)
       {
         c=msg2server.mtext[i];
        if(c>='a'&& c<='z')
        {msg2client.mtext[i]=toupper(c);}
        else if(c>='A'&& c<='Z') 
        {msg2client.mtext[i]=tolower(c);}  
        else
        {msg2client.mtext[i]=c;} 
      }


	     msg2client.mtype=msg2server.mtype;
	    
       //send the modified queue to the DN queue
    	if(msgsnd(DNmsqid,(void*)&msg2client,BUFFER_SIZE,0)==-1)
	     {
		      printf("error\n");
		      exit(1);
	     }

    }   
   return 0;
}