#include <signal.h>
#include <stdio.h>

void* my_handler(int nsig) 
{
  printf("HAHA! You cannot quit!\n");
}

int main(void)
{
  (void)signal(SIGINT, SIG_IGN);
  (void)signal(SIGQUIT, my_handler);

  while(1);
  return 0;
}
