/* Программа 2 для иллюстрации работы с очередями сообщений */

/* Эта программа получает доступ к очереди сообщений,
и читает из нее сообщения с любым типом в порядке FIFO до тех пор,
пока не получит сообщение с типом 255, которое будет служить
сигналом прекращения работы. */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main()
{
  int msqid; 

  char pathname[] = "ex0.c"; 

  key_t key; 

  struct mymsgbuf_in
  {
    long mtype;
    int input;
  } mybuf_one;

  struct mymsgbuf_out
  {
    long mtype;
    char output;
  } mybuf_two;	

  if((key = ftok(pathname, 0)) < 0)
  {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    printf("Can\'t get msqid\n");
    exit(-1);
  }

  
    if ((msgrcv(msqid, (struct msgbuf_in *)&mybuf_one, sizeof(int), 0, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }

    int result = 1;

    for(int i = 1; i <= mybuf_one.input; i++)
        result *= i;

    mybuf_two.mtype = 1;
    mybuf_two.output = result;

    if(msgsnd(msqid, (struct msgbuf_out *) &mybuf_two, sizeof(int), 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
  
  
  return 0;
} 
