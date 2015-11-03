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

    mybuf_one.mtype = 1;
    /*
     * FIXIT: не используйте в коде магические числа. Объявите константу N.
     * Просто если вы захотите изменить 5 на другое число, то изменения необходимо будет внести в нескольких частях кода.
     */
    mybuf_one.input = 5;    

    if(msgsnd(msqid, (struct msgbuf_in *) &mybuf_one, sizeof(int), 0) < 0)
    {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
      exit(-1);
    }
  

    if ((msgrcv(msqid, (struct msgbuf_out *)&mybuf_two, sizeof(int), 0, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);        
      exit(-1);
    }   

   printf("5! = %d", mybuf_two.output);

   msgctl(msqid, IPC_RMID, (struct msqid_ds*)NULL);
 
  return 0;
} 
