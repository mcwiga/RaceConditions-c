/* Adder_race_condition.c 1920-CS211, Week 9  */
/*
Name : Michael McGloin
ID : 18357636
Email : M.MCGLOIN2@nuigalway.ie
Compiler : https://www.onlinegdb.com/
Value of K : Quite likely at k >= 98 although race conditions did occur as low as k=20
*/



/* This program provides an insight into how implementing a semaphore can
    prevent a race condition.
   1. The program creates K child processes.
   2. The children try to sum 4 numbers by placing them in a pipe (inpipe). However
    they also check if another child is doing this at the same time by attempting
    to read from a different pipe (testPipe).
   3. If Child 1 is trying to sum numbers but Child 2 is in the middle of doing this
    Child 1 must wait until Child 2 has finished in whch case Child 2 will add a byte
    to the pipe (testPipe) for Child 1 to read.
   3. The parent then reads the numbers the children are attempting to sum, adds them,
      and sends the result back to the childreN via another pipe (outpipe): see the adder() function
   4. The children then reads this solution and prints it. 

However, without implementing our semaphore both children write to the pipe at the same time, so the numbers 
don't appear in the expected order. Therefore a RACE CONDITION occurs. 
*/

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>



int inpipe[2], outpipe[2] ,testPipe[2];  // These are global variables.

void adder(void);  // Computes the sum of 4 numbers in "inpipe"
                    // Puts answer into "outpipe"
              
int child(int a, int b, int c, int d); 
           // Puts four numbers to be summed into "inpipe"
           // Reads answer from "outpipe".
           
void Test(void); // reads from testPipe, if pipe is empty process must wait 
                // until something is written to it

void Increment(void); // writes a byte to testPipe

int main(void ) {

  int ParentsPID, ans,i;
  int k=98;
  pipe(inpipe);  
  pipe(outpipe);
  pipe(testPipe);
  Increment();


  ParentsPID = getpid(); // now I'll always know who I am
  
  for(i=0;i<k;i++){     // Create our K children
  if (getpid() == ParentsPID)
    fork(); // Make  a  child
  }
  
  for(i=0;i<k;i++){  // Let Parent call Adder K-times
  if ( getpid() == ParentsPID ) 
  {              /* Parent */
    adder();  
  }
  
  else      
  {              /* Child */
     Test();                // Try to read from testPipe
     ans=child(1,2,3,4);
     Increment();           // Write to testPipe
     if(ans!=10)
     {
        printf("Child (%d): 1+2+3+4= %d\n", getpid(), ans);
        
     }
     exit(0); // child should exit after printing the answer
  }
  }

  return(0);
}

void adder(void )
{
  int i, number, sum=0;

  for (i=0; i<4; i++)
  {
    read(inpipe[0], &number, sizeof(int));
    sum += number;
  }
  write(outpipe[1], &sum, sizeof(int));
}
  
int child(int a, int b, int c, int d)
{
  int ans;
  write(inpipe[1], &a, sizeof(int));
  write(inpipe[1], &b, sizeof(int)); 
 // sleep(1);  Pause for a second to encourage race condition
  write(inpipe[1], &c, sizeof(int)); 
  write(inpipe[1], &d, sizeof(int));

  read(outpipe[0], &ans, sizeof(int));
  return(ans);
}
void Test(void)
{
    int lock;
    read(testPipe[0],&lock,sizeof(int)); // try to read from testPipe
}

void Increment(void)
{
    int sem =1;
    write(testPipe[1], &sem, sizeof(int)); // write a byte to testPipe
}

    