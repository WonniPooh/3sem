#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  int nodes_amnt = 5;
  printf("Enter please nodes amount:\n");
  int current_fork_lvl = 0;
  scanf("%d", &nodes_amnt);
  pid_t pid = fork();
  int wait_status = 0;
  
  for(current_fork_lvl = 1; current_fork_lvl <= nodes_amnt; current_fork_lvl++)
  {
    if(pid == 0)
    {
       printf("I am child with id %d of pr. with UID %d, current fork lvl is %d\n", getpid(), getppid(), current_fork_lvl);
       
       if(current_fork_lvl < nodes_amnt)
       {
          pid = fork();
       }
       else
       {
           printf("I am the last child with id %d of pr. with UID %d, exiting with a code %d\n", getpid(), getppid(), 0);
           exit(0);
       }
    }
    else 
    {
       wait(&wait_status);
       printf("I am node with id %d, current fork lvl is %d\n", getpid(), current_fork_lvl);
       printf("Exiting with a code %d\n", 0);
       exit(0);
    }
  }
}
