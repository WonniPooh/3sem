#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>

#define THREADS_AMNT 2
int threads_used[THREADS_AMNT];

typedef struct DATA_T
{		
	int num_one;
	int num_two;
	long my_id;
}mydata_t;

typedef struct mymsgbuf_in
{
	long mtype;
	mydata_t data;
}mybuf_in_t;

typedef struct mymsgbuf_out
{
	long mtype;
	int result;
}mybuf_out_t;

typedef struct thread_input
{
	mybuf_in_t buf_in;
	int thread_num;
	int msgid;
	int semid;
}thread_t;

void* my_thread(void* dummy) 
{
    thread_t* input = (thread_t*)dummy; 
    
    mybuf_out_t output = {};
    
    struct sembuf destroy_thread = {};
   
    destroy_thread.sem_num = 0;													
    destroy_thread.sem_op =  1;												
    destroy_thread.sem_flg = 0;
    
    output.mtype = input -> buf_in.data.my_id;
    
    output.result = input -> buf_in.data.num_one * input -> buf_in.data.num_two;
             
    if(msgsnd(input -> msgid, (mybuf_out_t *) &output, sizeof(int), 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(input -> msgid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
    
   threads_used[input -> thread_num] = 0;
   
   semop(input -> semid, &destroy_thread, 1);
 
   return NULL;
}

int main()
{
	int msquid = 0; 
	int semid = 0;
	int result = 0;
	int successfull_thread_creation = 0;
	key_t key = 0; 

	char pathname[] = "ex0.c"; 
		
	pthread_t thread_id;
	thread_t * func_argument = (thread_t * )calloc(THREADS_AMNT, sizeof(thread_t));

	if((key = ftok(pathname, 0)) < 0)
	{
		printf("Can\'t generate key\n");
		exit(-1);
	}
	
    if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
    {
      printf("Can\'t get msqid\n");
      exit(-1);
    }
    
    struct sembuf initialize_sem = {};
   
    initialize_sem.sem_num = 0;													
    initialize_sem.sem_op =  THREADS_AMNT + 1;												
    initialize_sem.sem_flg = 0;
    
    struct sembuf are_there_free_threads = {};
  
    are_there_free_threads.sem_num = 0;													
    are_there_free_threads.sem_op =  -1;													
	are_there_free_threads.sem_flg = 0;
    
    semop(semid, &initialize_sem, 1);

	if((msquid = msgget(key, 0666 | IPC_CREAT)) < 0)
	{
		printf("Can\'t get msquid\n");
		exit(-1);
	}
	
	mybuf_in_t mybuf_input = {};
	
	while(1)
	{  		
		if ((msgrcv(msquid, (mybuf_in_t *) &mybuf_input, sizeof(mydata_t), 0, 0)) < 0)
		{
			printf("Can\'t receive message from queue\n");
			exit(-1);
		}
		else
		{
			printf("I recieved message! Process id is %d\n", mybuf_input.data.my_id);
			sleep(20);

			if(mybuf_input.mtype == 255)
				exit(0);
				
			semop(semid, &are_there_free_threads, 1);

   			for(int i = 0; i < THREADS_AMNT; i++)
			{
				if(threads_used[i] == 0)
				{	
					threads_used[i] = 1;
					
					func_argument[i].buf_in = mybuf_input;
					func_argument[i].thread_num = i;
					func_argument[i].msgid = msquid;
					func_argument[i].semid = semid;
										
					result = pthread_create(&thread_id, (pthread_attr_t *)NULL, my_thread, func_argument + i);

					if (result) 
					{
						printf("Can`t create thread, returned value = %d\n" , result);
						exit(-1);
					}
					
					break;
				}
			}	
		}
	}
  
  return 0;
} 
