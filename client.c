#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int msqid; 

  char pathname[] = "ex0.c"; 

  key_t key; 

  struct mymsgbuf_in
  {
    long mtype;
    
    struct data
    {		
		int num_one;
		int num_two;
		long my_id;
	}my_data;
  
  } mybuf_one;

  struct mymsgbuf_out
  {
    long mtype;
    int result;
    	
  } mybuf_two;

  if ((key = ftok(pathname, 0)) < 0)
  {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

    int input_num_one = 0;
    int input_num_two = 0;

    printf("Enter please two numbers you would like to multiply:\n");
    scanf("%d", &input_num_one);
    scanf("%d", &input_num_two);

    mybuf_one.mtype = 1;
    mybuf_one.my_data.my_id = getpid();    
    mybuf_one.my_data.num_one = input_num_one;
    mybuf_one.my_data.num_two = input_num_two;
 
    if(msgsnd(msqid, (struct msgbuf_in *) &mybuf_one, sizeof(struct data), 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
  

    if ((msgrcv(msqid, (struct msgbuf_out *)&mybuf_two, sizeof(int), getpid(), 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);        
      exit(-1);
    }   

   printf("%d * %d = %d\n", input_num_one, input_num_two, mybuf_two.result);
 
  return 0;
} 
