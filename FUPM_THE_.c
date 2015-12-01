#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int nsig)
{
	
}

int main(void)
{
	int sig;
	sigset_t sigset;
	sigset_t empty_sigset;
	sigemptyset(&sigset);
	sigemptyset(&empty_sigset);
	sigaddset(&sigset, SIGUSR1);
	sigaddset(&sigset, SIGUSR2);
	sigprocmask(SIG_SETMASK, &sigset, &empty_sigset);    
    pid_t pid = fork();      

    if (pid == 0)
    {
		pid = getppid();
		signal(SIGUSR1, SIG_IGN);
				
        while(1)
        {
            printf("FUPM\n");
            kill(pid, SIGUSR2);
            sigwait(&sigset, &sig);   
        }  
    }
    else
    {
		signal(SIGUSR2, SIG_IGN);
	
        while(1)
        {
            sigwait(&sigset, &sig);            
            printf("champion \n");
            kill(pid, SIGUSR1);     
        } 
    }
    return 0;
} 
