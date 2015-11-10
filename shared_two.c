#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

const int BUF_SIZE_MILTIPY_CONST = 2;

int main()
{
   int     semid = 0;
   char*   shared_array_pointer = NULL;
   int     shmid = 0; 
   const char pathname[] = "shared_one.c";
   key_t   key; 

   struct sembuf can_i_work_with_memory = {};
   
    can_i_work_with_memory.sem_num = 0;													
    can_i_work_with_memory.sem_op =  -1;												
    can_i_work_with_memory.sem_flg = 0;

    if((key = ftok(pathname , 0)) < 0)
    {
      printf("Can\'t generate key\n");
      exit(-1);
    }

    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
      printf("Can\'t get msqid\n");
      exit(-1);
    }
   
	FILE*  fMyFile = fopen("shared_one.c","r"); 
	fseek(fMyFile,0,SEEK_END); 
	long size = ftell(fMyFile); 
	fclose(fMyFile);
		
	int counter = 0;
	
    if((shmid = shmget(key, BUF_SIZE_MILTIPY_CONST * size * sizeof(char), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {        
        if(errno != EEXIST) 
        {  
			printf("Can't create shared memory\n");
            exit(-1);
        } 
        else 
        {			
            if((shmid = shmget(key, BUF_SIZE_MILTIPY_CONST * size * sizeof(char), 0)) < 0)
            {
               printf("Can't find shared memory\n");
               exit(-1);
            }
        }
    }
  
   if((shared_array_pointer = shmat(shmid, NULL, 0)) == (-1))
   {
      printf("Can't attach shared memory\n");
      exit(-1);
   }
 
   semop(semid, &can_i_work_with_memory, 1);

	while(shared_array_pointer[counter] != EOF )
		printf("%c",shared_array_pointer[counter++]);
		
	semctl(semid, 1, IPC_RMID);
   
   if(shmdt(shared_array_pointer) < 0)
   {
      printf("Can't detach shared memory\n");
      exit(-1);
   }
   
   if(shmctl(shmid, IPC_RMID, NULL) < 0)
   {
      printf("Can't delete shared memory\n");
      exit(-1);
   }

   return 0;
}
