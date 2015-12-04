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

  /*
   * FIXIT:
   * Label`ы не надо использовать никогда. Совсем-совсем никогда.
   * С их помощью можно совершенно запутать логику программы.
   * Можно реализовать любой алгоритм без их использования.
   */
  
loop:   
  if(pid == 0)
  {
     printf("I am child with id %d of pr. with UID %d, current fork lvl is %d\n", getpid(), getppid(), current_fork_lvl);
     current_fork_lvl++;
     if(current_fork_lvl < nodes_amnt)
     {
        pid = fork();
        goto loop;
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
