#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>


void  execute(char **argv)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                                  /* for the parent:      */
          while (wait(&status) != pid)       /* wait for completion  */
               ;
     }
}


int main(int argc,char* argv[])

{

	char* query = (char*) malloc(100*sizeof(char)) ;  
	char* location  = (char*) malloc(100*sizeof(char));
	char** arglist = (char**)malloc(10*sizeof(char*));
	char* pPath;
    pPath = getenv ("PATH");
	int arglist_number = 0;
	int location_length = 0;
	int i,j;
	char cwd[1024];
	DIR *dir;
    struct dirent *entry;
    char* s ;
    struct stat statbuf;
    int pid;


	for(i=0;i<100;i++)
	{
		query[i] = '\0' ;
		location[i] = '\0';
	}

	while(1)
	{
		free(query);
		free(location);
		free(arglist);

		query = (char*) malloc(100*sizeof(char)) ;
		location = (char*) malloc(100*sizeof(char));
		arglist = (char**)malloc(10*sizeof(char*));

		arglist_number = 0;
		location_length = 0;


		for(i=0;i<100;i++)
	{
		query[i] = '\0' ;
		location[i] = '\0';
	}


		printf(">myshell\n");
		gets(query);
		query[strlen(query)] = '\0';
		if(strcmp(query,"exit") == 0)
			exit(0);
		//printf("Read - %s\n",query );

		if(query[0] == 'p' && query[1] == 'w' && query[2] == 'd' && query[3] == '\0')
		{

			if (getcwd(cwd, sizeof(cwd)) != NULL)
           fprintf(stdout, "Current working dir: %s\n", cwd);
            else
           perror("getcwd() error");
           continue;
		}
		else if(query[0] == 'l' && query[1] == 's' && query[2] == '\0')
		{
			s = (char*)getcwd(cwd, sizeof(cwd));
			if ((dir = opendir(s)) == NULL)

    		perror("opendir() error");
  			else {
    		puts("contents of directory:");
    		while ((entry = readdir(dir)) != NULL){
      		printf("%s\n", entry->d_name);
      		 }

    		closedir(dir);
    		continue;
  		}

		}
		else if(query[0] == 'l' && query[1] == 's' && query[2] == '-' && query[3] == 'l')
		{
			s = (char*)getcwd(cwd, sizeof(cwd));
			if ((dir = opendir(s)) == NULL)

    		perror("opendir() error");
  			else {
    		puts("contents of directory:");
    		while ((entry = readdir(dir)) != NULL){
      		printf("*********\n");
      		printf("Name of the file   	  - %s\n", entry->d_name);
      		stat(entry->d_name, &statbuf);
      		printf("Device id                 - %ld\n",statbuf.st_dev);
      		printf("File serial number        - %ld\n",statbuf.st_ino);
      		printf("Mode of File              - %ld\n",statbuf.st_mode );
      		printf("User id of file           - %ld\n",statbuf.st_uid);
      		printf("File size in bytes        - %ld\n",statbuf.st_size);
      		printf("Time of last access       - %ld\n",statbuf.st_atime);
      		printf("Time of last modification - %ld\n\n",statbuf.st_mtime);
      		 }

    		
		        }
		    closedir(dir);
    		continue;

        }
        else if(query[0] == 'c' && query[1] == 'd' && query[2] == '<'  )
        {

        	i=3;
        	while(query[i] != '>')
        	{
        		location[location_length] = query[i];
        		i++;
        		location_length++;

        	}

        	if( chdir(location) == 0)
        	{
        		printf("Directory is changed! to the specified one!\n");
        	}
        	else
        	{
        		printf("Error in chdir() \n");
        	}

        	continue;

        }
        else if(query[0] == 'm' && query[1] == 'k' && query[2] == 'd' && query[3] == 'i' && query[4] == 'r' && query[5] == '<')
        {

        	i=6;
        	while(query[i] != '>')
        	{
        		location[location_length] = query[i];
        		i++;
        		location_length++;

        	}
        	if( mkdir(location,S_ISVTX) == 0)
        	{
        		printf("New folder created!\n");
        	}
        	else
        	{
        		printf("Error in mkdir()!\n");
        	}

        	continue;
        }
        else if(query[0] == 'r' && query[1] == 'm' && query[2] == 'd' && query[3] == 'i' && query[4] == 'r' && query[5] == '<')
        {

        	i=6;
          
        	while(query[i] != '>')
        	{
        		location[location_length] = query[i];
        		i++;
        		location_length++;

        	}
        	if( rmdir(location) == 0)
        	{
        		printf("Folder deleted!\n");
        	}
        	else
        	{
        		printf("Error in rmdir()!\n");
        	}

        	continue;
        }
       else 
        {
        	i=0;
        	arglist[0] = (char*) malloc(10*sizeof(char));
        	if(strcmp(query,"a.out") == 0)
        	{
        		//printf("here!\n");
        		arglist[0][0] = '.';
        		arglist[0][1] = '/';
        		arglist[0][2] = 'a';
        		arglist[0][3] = '.';
        		arglist[0][4] = 'o';
        		arglist[0][5] = 'u';
        		arglist[0][6] = 't';
        		arglist[0][7] = '\0';
                arglist[1] = NULL;

        		execute(arglist);
        		continue;
        	}


        	while(1)
        	{

        	//	printf("Entering %dthe time\n",arglist_number );
        		arglist[arglist_number] = (char*)malloc(10*sizeof(char));
        		if(query[i] == '\0') break;
        		j=0;
        		while(1)
        		{
        			if(query[i] == ' ' || query[i] == '\0') break;
        	//		printf("entering inner while loop\n");
        			arglist[arglist_number][j] = query[i];
        			i++;
        			j++;
        		}
        	//	printf("left innner while loops\n");
        		i++;

        		arglist[arglist_number][j] = '\0' ;
        		//printf("Read the list - %s\n",arglist[arglist_number]);
        		arglist_number++;

        	}



        	/*for(i=0;i<arglist_number;i++)
        	{
        		printf("%s\n",arglist[i] );
        	}*/
        //	printf("Searching in the path ........... \n %s\n",pPath);
        	arglist[arglist_number] == NULL;
			execute(arglist);
        	continue;

       }

		
	}


	return 0;

	}
