#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>

/*This is used to freeze the XTERM */
int global_pid;

/*This generates the random numbers */
int generate_random()
{

return (  rand() % 128 );

}
/*This calculates the maximum element in an array within the given bounds. */
int calculate_max_element(int*A,int L,int R)
{
int max_element = -1;
int iterator;
for(iterator = L;iterator <= R;iterator++)
{

	if(A[iterator] > max_element)
		max_element = A[iterator];


}

return(max_element);

}
/*This function calculates the maximum element by forking 
  This will be repetattively called for calculation of
  the maximum elements in chunk */
void calculate_max_forking(int*A,int L,int R)
{

   int NumberInChunk = R-L+1; //number of elements
   //static int M = 0;
   int M;
   int pid,ppid,MaxElement;
   int pid1,pid2,status1,status2;
   pid = getpid();
   ppid = getppid();
   if(NumberInChunk < 10) // default case
   {
   	   MaxElement = calculate_max_element(A,L,R);
       printf("The maximum element element for this process is %d with pid as %d and ppid as %d\n",MaxElement,pid,ppid );
       exit(MaxElement); //exit using the maxelemnt value,its parent will catch it and use this max value for comparisson.

   }
   else{
   			M = (int) (L+R)/2 ;  //calculate middle element

   			if ((pid1 = fork()) == 0) {

			       calculate_max_forking(A,L,M);  //call the function recursively for the left half.   			 
             sleep(1) ; /* Sleep for 1 second */
    		 
            }

            else {

           if ((pid2 = fork()) == 0) {

              calculate_max_forking(A,M+1,R); //call the function recursively for the second half.
              sleep(1) ; /* Sleep for 1 second */
                    }
    
           else {

      
      
              sleep(1) ; /* Sleep for 1 second */
      

      
              waitpid(pid1, &status1, 0);
              waitpid(pid2, &status2, 0); // wait till the children complete

              if(WEXITSTATUS(status1) >= WEXITSTATUS(status2) ) //calculate the maximum element
              	MaxElement = WEXITSTATUS(status1);
              else
              	MaxElement = WEXITSTATUS(status2);

              
              printf("The maximum element element for this process is %d with pid as %d and ppid as %d\n",MaxElement,pid,ppid );
              if(global_pid == pid)  // If it is the main process which is the root then we have got the maximum value.
                { printf("Finished process  %d\n",MaxElement);

                sleep(5); // This is used for the XTERM to freeze for sometime.
               }
              exit(MaxElement);
              

              }
 

                 }


       }

}



int main(int argc,char* argv[])

{
   srand(time(NULL));
   int NumberOfIntegers = atoi(argv[1]),pid,ppid,L,R;
   int MaxElement;
   int* A = (int*)malloc(NumberOfIntegers * sizeof(int));
   int A_iterator;
   
   for(A_iterator = 0 ; A_iterator < NumberOfIntegers ; A_iterator++) //Fill the array
   {

   		A[A_iterator] = generate_random();

   }
   pid = getpid();
   global_pid = pid;
   ppid = getppid();
   printf("\nParent Process pid = %d , ppid = %d\n",pid,ppid);
   printf("The array values are as follows \n");
   for(A_iterator = 0 ; A_iterator < NumberOfIntegers ; A_iterator++) //Print for debugging
   {

   		printf("%d\n",A[A_iterator]); 

   }
   L = 0;
   R = NumberOfIntegers - 1;
   calculate_max_forking(A,L,R); // call the function with the proper bounds.
   
   printf("The maximum element element for this process as parent process is %d with pid as %d and ppid as %d\n",MaxElement,pid,ppid );
   
   return 0;
}
