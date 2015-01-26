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
#include <fcntl.h>
#include <time.h>


void  execute(char **argv)  // takes arglist and exec's the child process
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

void childSignalHandler(int sig) // signal handler for ctrl + c
{


printf("\n>myshell\n");


}

int main(int argc,char* argv[])

{

  signal(SIGINT,childSignalHandler);
	char* query = (char*) malloc(100*sizeof(char)) ;  
	char* location  = (char*) malloc(100*sizeof(char));
	char** arglist = (char**)malloc(10*sizeof(char*));
	char* pPath;
  char* output_file = (char*)malloc(20*sizeof(char));
  int output_file_length = 0;
  char* input_file = (char*)malloc(20*sizeof(char));
  int input_file_length = 0;
  char* common_file = (char*)malloc(20*sizeof(char));
  strcpy(common_file,"common.txt");

  char** arglist1 = (char**)malloc(2*sizeof(char*));
  char** arglist2  = (char**)malloc(2*sizeof(char*));

  arglist1[0] = (char*)malloc(10*sizeof(char));
  arglist2[0] = (char*)malloc(10*sizeof(char));
  arglist1[1] = (char*)malloc(10*sizeof(char));
  arglist2[1] = (char*)malloc(10*sizeof(char));
  int redirection_flag = 0;
  int pid;
  int pid1,pid2;
  int background_flag = 0;
  int output_flag = 0;
  int input_flag = 0;
    pPath = getenv ("PATH");
	int arglist_number = 0;
	int location_length = 0;
	int i,j;
	char cwd[1024];
	DIR *dir;
  struct dirent *entry;
  char* s ;
  struct stat statbuf;
  int outputfd;
  int inputfd;

    


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
    free(input_file);
    free(output_file);
     
		query = (char*) malloc(100*sizeof(char)) ;
		location = (char*) malloc(100*sizeof(char));
		arglist = (char**)malloc(10*sizeof(char*));
    output_file = (char*)malloc(20*sizeof(char));
    input_file = (char*)malloc(20*sizeof(char));

		arglist_number = 0;
		location_length = 0;
    input_file_length = 0;
    output_file_length = 0;


		for(i=0;i<100;i++)
	{
		query[i] = '\0' ;
		location[i] = '\0';
	}

   for(i=0;i<20;i++)
   {
    input_file[i] = '\0';
    output_file[i] = '\0';

   }

		printf(">myshell\n");
		gets(query);
    if(feof(stdin))   // handler for ctrl + d
      exit(0);
		query[strlen(query)] = '\0';
		if(strcmp(query,"exit") == 0)
			exit(0);
		//printf("Read - %s\n",query );

		if(query[0] == 'p' && query[1] == 'w' && query[2] == 'd' && query[3] == '\0') //pwd
		{

			if (getcwd(cwd, sizeof(cwd)) != NULL)
           fprintf(stdout, "Current working dir: %s\n", cwd);
            else
           perror("getcwd() error");
           continue;
		}
		else if(query[0] == 'l' && query[1] == 's' && query[2] == '\0') //ls
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

          // Back ground process!!
          if(query[0] == 'a' && query[1] == '.' && query[2] == 'o' && query[3] == 'u' && query[4] == 't' && query[5] == ' ' && query[6] == '&' )
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

            if(  (pid = fork()) == 0 )
            {
              printf("In child process - \n");

              execute(arglist);

              exit(0);


            } 

            else
            {

              continue;

            }




          }

         // if()   -- do it separatley for a.out as well



        	while(1)  // handles all the extra cases
        	{

        	//	printf("Entering %dthe time\n",arglist_number );
        		
        		if(query[i] == '\0') break;       
            if(query[i] == '&')         // background process handler
             {
              background_flag = 1;
              i++;
              continue;
             }
             if(output_flag == 1)       // output file
             {
               while(query[i] != '\0')
               {
                output_file[output_file_length] = query[i];
                i++;
                output_file_length++;
               }
               output_file[output_file_length] = '\0' ;
                printf("The %s\n",output_file );
               break;

             }

             if(input_flag == 1 && output_flag == 0 && redirection_flag == 0 && query[i] != '>' && query[i] != '|') // handle input file
             {
               while(query[i] != '\0' && query[i] != ' ')
               {
                input_file[input_file_length] = query[i];
                i++;
                input_file_length++;
               }
               input_file[input_file_length] = '\0' ;

               printf("The %s\n",input_file );
               if(query[i] == '\0') break;
               else
               {
                i++;
                continue;
               }
               

             }

             if(query[i] == '>')
              {
                output_flag = 1;
                i++;
                i++;
                continue;
              }
             if(query[i] == '<')
             {
              input_flag = 1;
              i++;
              i++;
              continue;
             }
             if(query[i] == '|')   // piping
             {

              redirection_flag = 1;
              i+=2;
              continue;
             }

            arglist[arglist_number] = (char*)malloc(10*sizeof(char));
        		j=0;
        		while(1)   // read the arguments and store it in argument list
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
        		printf("Read the list - %s\n",arglist[arglist_number]);
        		arglist_number++;

        	}



        	/*for(i=0;i<arglist_number;i++)
        	{
        		printf("%s\n",arglist[i] );
        	}*/
        //	printf("Searching in the path ........... \n %s\n",pPath);
        	arglist[arglist_number] == NULL;

// take input form various files and store it in various files depending on the combination of 3 flags

          
          if(redirection_flag == 0){

          if(output_flag == 0)
          {
            if(input_flag == 0)
          {
            execute(arglist);
            continue;
          }
          else
          {

            if((pid2 = fork()) == 0) {
            printf("yey\n");
           // sleep(1);
           printf("The input_file -%c\n",input_file[1] );
           inputfd = open(input_file,O_RDONLY);
           if (inputfd < 0) {
            fprintf(stderr, "Unable to open input file in read mode...\n");
            exit(1);
            } else {
             fprintf(stderr, "New file descriptor obtained = %d\n", inputfd);
            }

            

            close(0);
            dup(inputfd);
            close(inputfd);
            // execute(arglist);
             execvp(*arglist, arglist);
             exit(0);
            }
           else{
           //   sleep(1);
              wait(NULL);
            //  printf("compe--------\n");
              input_flag = 0;
              continue;
            }

          }
          }
          else
          {
            if(input_flag == 0){
            if((pid1 = fork()) == 0) {
        //    printf("yey\n");
           // sleep(1);
            outputfd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
            if (outputfd < 0) {
                perror("open(2) file: OUTPATH");
                exit(EXIT_FAILURE);
               }

            if (close(STDOUT_FILENO) < 0) {
                perror("close(2) file: STDOUT_FILENO");
                close(outputfd);
                exit(EXIT_FAILURE);
              }
             if (dup(outputfd) != STDOUT_FILENO) {
               perror("dup(2)");
               close(outputfd); /*N.B. Remember to close your files!*/
               exit(EXIT_FAILURE);
             }

             close(outputfd);
            // execute(arglist);
             execvp(*arglist, arglist);
             exit(0);
            }



           else{
           //   sleep(1);
              wait(NULL);
            //  printf("compe--------\n");
              output_flag = 0;
              continue;
            }
          }
          else
          {

            if((pid2 = fork()) == 0) {


           inputfd = open(input_file,O_RDONLY);
           if (inputfd < 0) {
            fprintf(stderr, "Unable to open input file in read mode...\n");
            exit(1);
            } else {
             fprintf(stderr, "New file descriptor obtained = %d\n", inputfd);
            }
            outputfd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
            if (outputfd < 0) {
                perror("open(2) file: OUTPATH");
                exit(EXIT_FAILURE);
               }

            close(0);
            close(1);

            dup(inputfd);
            close(inputfd);

            dup(outputfd);
            close(outputfd);

            execvp(*arglist, arglist);
             exit(0);



            }
            else
            {

              wait(NULL);
              output_flag = 0;
              input_flag = 0;
              continue;
            }



          }

          }

          
        }

        else{

            if((pid1 = fork()) == 0) {
        //    printf("yey\n");
           // sleep(1);
            if(input_flag == 1)
            {
              inputfd = open(input_file,O_RDONLY);
           if (inputfd < 0) {
            fprintf(stderr, "Unable to open input file in read mode...\n");
            exit(1);
            } else {
             fprintf(stderr, "New file descriptor obtained = %d\n", inputfd);
            }

            

            close(0);
            dup(inputfd);
            close(inputfd);


            }
            outputfd = open("common.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
            if (outputfd < 0) {
                perror("open(2) file: OUTPATH");
                exit(EXIT_FAILURE);
               }

            if (close(STDOUT_FILENO) < 0) {
                perror("close(2) file: STDOUT_FILENO");
                close(outputfd);
                exit(EXIT_FAILURE);
              }
             if (dup(outputfd) != STDOUT_FILENO) {
               perror("dup(2)");
               close(outputfd); /*N.B. Remember to close your files!*/
               exit(EXIT_FAILURE);
             }

             close(outputfd);
            // execute(arglist);
             strcpy(arglist1[0],arglist[0]);
             arglist1[1] = NULL;
             execvp(*arglist1, arglist1);
             exit(0);
            }



           else{
           //   sleep(1);
              wait(NULL);
            //  printf("compe--------\n");
              if((pid2 = fork()) == 0) {
            printf("yey\n");
           // sleep(1);
         //  printf("The input_file -%c\n",input_file[1] );
            if(output_flag == 1){

              outputfd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRGRP | S_IROTH);
            if (outputfd < 0) {
                perror("open(2) file: OUTPATH");
                exit(EXIT_FAILURE);
               }

            if (close(STDOUT_FILENO) < 0) {
                perror("close(2) file: STDOUT_FILENO");
                close(outputfd);
                exit(EXIT_FAILURE);
              }
             if (dup(outputfd) != STDOUT_FILENO) {
               perror("dup(2)");
               close(outputfd); /*N.B. Remember to close your files!*/
               exit(EXIT_FAILURE);
             }

             close(outputfd);


            }
           inputfd = open("common.txt",O_RDONLY);
           if (inputfd < 0) {
            fprintf(stderr, "Unable to open input file in read mode...\n");
            exit(1);
            } else {
             fprintf(stderr, "New file descriptor obtained = %d\n", inputfd);
            }

            

            close(0);
            dup(inputfd);
            close(inputfd);
            // execute(arglist);
            strcpy(arglist2[0],arglist[1]);
             arglist2[1] = NULL;
             execvp(*arglist2, arglist2);
             exit(0);
            }
           else{
           //   sleep(1);
              wait(NULL);
            //  printf("compe--------\n");
             // input_flag = 0;
              redirection_flag = 0;
              input_flag = 0;
              output_flag = 0;
              continue;
            }




             
            }




        }

          if(background_flag == 0) //foreground process
          {  
			    execute(arglist);
          continue;
          }
          else                     //background process
          {

            if(  (pid = fork()) == 0 )
            {
              printf("In child process - \n");

              execute(arglist);

              exit(0);


            } 

            else
            {

              continue;

            }


          }

          




        	continue;

       }

		
	}


	return 0;

	}
