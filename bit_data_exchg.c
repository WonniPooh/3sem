#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define STRLEN_TO_SEND 255

void send_the_char(unsigned char current_char, sigset_t* sigset, int* sig, pid_t pid);
char recieve_the_char(sigset_t* sigset, int* sig, pid_t pid);

int main()
{
  int sig;
  sigset_t sigset, empty_sigset;

  sigemptyset(&sigset);
  sigemptyset(&empty_sigset);

  sigaddset(&sigset, SIGUSR1);
  sigaddset(&sigset, SIGUSR2);

  sigprocmask(SIG_SETMASK, &sigset, &empty_sigset);    

  pid_t pid = fork();      

  if (pid == 0)
  {
    pid = getppid();
    int strlength = 0;    
    char str_to_send[STRLEN_TO_SEND] = {};
   
    printf("Enter a string you would like to send: (%d length max)\n", STRLEN_TO_SEND);
    scanf("%s", str_to_send);
   
    strlength = strlen(str_to_send);
     
    send_the_char((unsigned char)strlength, &sigset, &sig, pid);
   
    for(int i = 0; i < strlength; i++)
    {   
      send_the_char(str_to_send[i], &sigset, &sig, pid);
    }  
  }
  else
  {
    int strlength = 0;
    char recieved_string[STRLEN_TO_SEND] = {};

    strlength = recieve_the_char(&sigset, &sig, pid);
   
    for(int i = 0; i < strlength; i++)
    {
      recieved_string[i] = recieve_the_char(&sigset, &sig, pid);
    }    
    printf("string recieved: %s\n",recieved_string);
    wait(NULL);
  }
  return 0;
} 

void send_the_char(unsigned char current_char, sigset_t* sigset, int* sig, pid_t pid)
{  
  char current_bit = 0;

  for(int j = 0; j < 8; j++)
  {
    current_bit = current_char & 1;
  
    kill(pid, current_bit ? SIGUSR1 : SIGUSR2); //всё никак к нему не привыкну и не запомню как он раотает. Согласен, спасибо.
    
    current_char = current_char >> 1;  
    sigwait(sigset, sig); 
  }
}

char recieve_the_char(sigset_t* sigset, int* sig, pid_t pid)
{  
  char current_char = 0;
  
  for(int j = 0; j < 8; j++)
  {
    sigwait(sigset, sig);
    
    if(*sig == SIGUSR1)
    {
      current_char = current_char | (1 << j);
    }
    else if(*sig == SIGUSR2)
    {
      current_char = current_char & ~(1 << j);
    }
    kill(pid, SIGUSR1);
  }
  return current_char;
}
