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
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
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
	key_t key;
	int thread_num;
	
}thread_t;

void* my_thread(void* dummy) 
{
	int msquid = 0;
	
    thread_t* input = (thread_t*)dummy; 
    
    mybuf_out_t output = {};
    
    output.mtype = input -> buf_in.data.my_id;
    
    output.result = input -> buf_in.data.num_one * input -> buf_in.data.num_two;
    
    if((msquid = msgget(input -> key, 0666 | IPC_CREAT)) < 0)
	{
		printf("Can\'t get msquid\n");
		exit(-1);
	}
     
    pthread_mutex_lock(&mutex); 
    
    if(msgsnd(msquid, (mybuf_out_t *) &output, sizeof(int), 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msquid, IPC_RMID, (struct msqid_ds*)NULL);
      pthread_mutex_unlock(&mutex);
      exit(-1);
    }
   
   pthread_mutex_unlock(&mutex);

   input -> thread_num = 0;
   
   threads_used[input -> thread_num] = 0;
   
   return NULL;
}

int main()
{
	int msquid = 0; 
	int result = 0;
	int successfull_thread_creation = 0;
	key_t key = 0; 

	char pathname[] = "ex0.c"; 
		
	pthread_t thread_id;
	thread_t * funk_argument = (thread_t * )calloc(THREADS_AMNT, sizeof(thread_t));

	if((key = ftok(pathname, 0)) < 0)
	{
		printf("Can\'t generate key\n");
		exit(-1);
	}

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
loop:

			if(mybuf_input.mtype == 255)
				exit(0);

			for(int i = 0; i < THREADS_AMNT; i++)
			{
				if(threads_used[i] == 0)
				{	
					threads_used[i] = 1;
					successfull_thread_creation = 1;
					
					funk_argument[i].buf_in = mybuf_input;
					funk_argument[i].thread_num = i;
					funk_argument[i].key = key;
										
					result = pthread_create(&thread_id, (pthread_attr_t *)NULL, my_thread, funk_argument + i);

					if (result) 
					{
						printf("Can`t create thread, returned value = %d\n" , result);
						exit(-1);
					}
					
					break;
				}
			}	
			
			if(successfull_thread_creation == 1)	
				successfull_thread_creation = 0;
			else
				goto loop;
		}
	}
  
  return 0;
} 
