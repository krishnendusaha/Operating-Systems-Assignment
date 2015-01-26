#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define MAX_SIZE 10       //max chunk size that may be sorted by a process
#define MAX_LEVEL 10      //max level upto which a process may be forked to produce 2 childs 

//This generates the random numbers 
int generate_random()
{

return (  rand() % 1000 );

}

// this function tkes  a shared memory portion array between index L and R  and makes it sorted  
void selection_sort(int shmid,int L,int R)   
{
   int * A= (int *) shmat(shmid, 0, 0);
   int i,j,temp;
   for(i=L;i<R;i++)
   {
    for(j=i+1;j<=R;j++)
     {
        if(A[i]<A[j])
        {
         temp=A[i];
         A[i]=A[j];
         A[j]=temp; 
        }
     }
   }

}


void merge_sort(int shmid,int L,int R,int *level)
{
   
   int status1,status2;
   int NumberInChunk = R-L+1; //number of element
   int M,pid1,pid2,i,j,k;
   int t_shmid=shmget(IPC_PRIVATE, 2*sizeof(int), 0777|IPC_CREAT); // a shared memory portion for siblings communication
   int* x = (int *) shmat(t_shmid, 0, 0);                          
   int* y = (int *) shmat(shmid, 0, 0);
   x[0]=0,x[1]=0; 

   if(NumberInChunk <= MAX_SIZE||(*level)>MAX_LEVEL)  //if chunksize <MAX_SIZE  or level >MAX_LEVEL then sort it immediately without further breaking  
   {
       selection_sort(shmid,L,R);
   }
   else
   {      
           (*level)+=1;
            M = (int) (L+R)/2 ;  //calculate middle element

            if ((pid1 = fork()) == 0) {
                   int *arr=(int *)malloc(sizeof(int)*(M-L+1));  //local memory to store the first n/2 elements of parent array of size n
                  
                   merge_sort(shmid,L,M,level);                  //sort the first half of array individually
                   x[0]=1;                                       //status =1
                   while(x[1]==0){ usleep(10);}                  // wait until the other sibling comes to status 1
                 
                   if(x[1]>0)                                    //if other sibling is in status 1
                   {
                     j=L; k=M+1;                                 //find the first  n/2 elements of the actual array
                     for(i=0;i<=M-L;i++)                         // and store them in local memory
                      { 
                        if(y[j]<=y[k]) {arr[i]=y[k]; k++; } 
                        else {arr[i]=y[j]; j++;} 
                      }
                   } 
                   x[0]=2;                                       // status =2
                      
                   while(x[1]!=2){ usleep(10);}                  // wait if sibling process  still not reached status 2
                   if(x[1]==2)                                   // if the sibling has reached status 2
                   {                                             //then copy the local memory array to the shared memory
                    for(i=L;i<=M;i++){ y[i]=arr[i-L];}
                   }
                 
                  exit(1);
            }

            else 
            {
                    if ((pid2 = fork()) == 0) {
                          int *arr=(int *)malloc(sizeof(int)*(R-M));    //local memory to store the last n/2 elements of parent array of size n
                          merge_sort(shmid,M+1,R,level);                //sort the last half of array individually
                          
                          x[1]=1;                                       //status =1
                          while(x[0]==0){ usleep(10);}                  // wait until the other sibling comes to status 1
                         
                          if(x[0]>0)                                    //if other sibling is in status 1
                          {  
                             j=M; k=R;                                  //find the last  n/2 elements of the actual array
                             for(i=R-M-1;i>=0;i--)                      // and store them in local memory
                             {
                               if(y[j]<=y[k]) { arr[i]=y[j]; j--; }
                               else {arr[i]=y[k]; k--;} 
                             } 
                          }
                           x[1]=2;                                       // status =2
                                  
                           while(x[0]!=2){ usleep(10);}                  // wait if sibling process  still not reached status 2
                           if(x[0]==2)                                   // if the sibling has reached status 2
                           {                                             //then copy the local memory array to the shared memory
                            for(i=M+1;i<=R;i++){y[i]=arr[i-M-1];}
                           }
                      exit(1);
                    }
    
                   else {

              
                     waitpid(pid1, &status1, 0);                         //wait for both the child  processes to end  
                     waitpid(pid2, &status2, 0); 
                 
              
                  }
            }

            shmdt(x);

            shmctl(t_shmid, IPC_RMID, 0);

    }
}


int main(int argc,char* argv[])

{
  
   srand(time(NULL));
   int NumberOfIntegers = atoi(argv[1]),shmid,L,R,level=0;
   shmid = shmget(IPC_PRIVATE, NumberOfIntegers*sizeof(int), 0777|IPC_CREAT);
   int A_iterator;
   printf("%d\n",shmid );
   int *A=(int *) shmat(shmid, 0, 0);                                       //address to the shared memory to store the array
   printf("The Input Array:\n");
   for(A_iterator = 0 ; A_iterator < NumberOfIntegers ; A_iterator++)       //Fill the array
   {

   		A[A_iterator] = generate_random();
      printf("%4d ", A[A_iterator] );
      if((A_iterator+1)%20==0){printf("\n");}
   }
   printf("\n");
  
   
   L = 0;                                      
   R = NumberOfIntegers - 1;
   merge_sort(shmid,L,R,&level); 
   
  
   printf("\n\n*************************************************************************************************\n");

   printf("The Sorted Array:\n");
   for(A_iterator = 0 ; A_iterator < NumberOfIntegers ; A_iterator++) //Fill the array
   {
      printf("%4d ", A[A_iterator] );

      if((A_iterator+1)%20==0){printf("\n");}
   }
   shmdt(A);
   printf("\n****************************************************************************************************\n");

   shmctl(shmid, IPC_RMID, 0);
   return 0;
}


