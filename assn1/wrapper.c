/*
		EXECUTING AN APPLICATION USING EXEC COMMANDS

	The execlp() family of commands can be used to execute an
	application from a process. The system call execlp()
	replaces the executing process by a new process image
	which executes the application specified as its parameter.
	Arguments can also be specified. Refer to online man pages.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>

int main(int argc,char*argv[])
{
	  //Catch boundary case
	    if(argc != 2)
	    {

	    	printf("You did not enter the array size as a parameter!\n");
	    }
	    //
	execl("/usr/bin/xterm","/usr/bin/xterm","-e","./parmax",argv[1],(void*)NULL); 

	/* The execlp() system call does not return. Note that the
	   following statement will not be executed.
	*/

	printf("This statement is not executed if execlp succeeds.\n");
	return 0;
}