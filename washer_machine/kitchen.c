#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h> 

#define PREPARE_OP(SEM, NUM, OP, FLAG) SEM.sem_num = (NUM);\
SEM.sem_op = (OP);\
SEM.sem_flg = (FLAG);


#define FILL_THE_DISH(NUM, DRY_TIME, WASH_TIME) new_node -> dish.dish_name[5] += (NUM);\
new_node -> dish.time_to_dry = (DRY_TIME);\
new_node -> dish.time_to_wash = (WASH_TIME);

/*
 * Алексей, у вас очень большая программа + нет описания того, как её запускать.
 * Давайте вы на семинаре поясните, как программа устроена, и продемонстрируете работу.
 */

enum 
{
  waiter,
  washer,
  dryer
};

typedef struct KITCHEN_DISH
{
  char dish_name[20];
  int time_to_wash;
  int time_to_dry;
  int is_it_clean;
  int is_it_dry;
}dish_t;

typedef struct DISH_NODE
{
  dish_t dish;
  struct DISH_NODE* next;
  struct DISH_NODE* prev;
}node_t;

typedef struct DISH_LIST
{
  node_t* first;
  node_t* last;
  node_t* first_data;
  node_t* last_data;
  int elem_count;
}list_t;

typedef struct MYMSGBUF
{
  long mtype;
  node_t dish_node;
  
} mybuf_t;

int prole;

const char pathname[] = "kitchen.c";
const char pathname_to_wash[] = "dish_washer.h";
const char pathname_to_dry[] = "dish_dryer.h";
const int  WASHER_ARRAY_SIZE = 100;

void run_process(int argc, char* argv[]);
void dish_generator(int argc, char* argv[]);
void dish_washer(int argc, char* argv[]);
void dish_dryer(int argc, char* argv[]);

int sem_get_access(const char* my_pathname);
void delete_sem(int semid);

int shmem_get_access(const char* my_pathname, int size);
void delete_shmem(int shmid);

int queue_get_access(const char* my_pathname);
void delete_queue(int shmid);

node_t* attach_shmem(int shmid);
void detach_shmem(node_t* shared_array_pointer);

#include "dish_generator.h"
#include "dish_washer.h"
#include "dish_dryer.h"

int  main(int argc, char* argv[])
{ 
  pid_t pid = 1;
  int status;
  for(int i = 0; i < 3; i++)
  {
    if(pid > 0)
    {
      prole = i;
      if ((pid = fork()) < 0)
      {
        printf("Bad fork!\n");
        exit(1);
      }
    }
  }

  if (pid > 0)   /***** Parent *****/
  {
    for(int j = 0; j < 3; j++)
      wait(&status);
  }
  else
  {
    run_process(argc, argv);
  }
  
  return 0;
} 

void run_process(int argc, char* argv[])
{  
  switch(prole)
  {
    case 0:  dish_generator(argc, argv);   break;
    case 1:  dish_washer(argc, argv);      break;
    case 2:  dish_dryer(argc, argv);       break;
    default: printf("Bad process role!\n"); exit(-1);
  }
}

void dish_generator(int argc, char* argv[])
{

  if(argc > 1)
  {
    if(strcmp("sem", argv[1]) == 0)
    {
      generate_dishes_sem();
    }
    else if (strcmp("queue", argv[1]) == 0)
    {
      generate_dishes_queue();
    }
    else
    {
      printf("invalid cmd command 1 input\n");
      exit(1);
    }
  }
  else
  {
    printf("Not enough arguments!\n");
    exit(-1);
  }
}

void dish_washer(int argc, char* argv[])
{
  if(!strcmp("sem", argv[1]))
  {
    recieve_dishes_sem(argv);
  }
  else if(!strcmp("queue", argv[1]))
  {
    recieve_dishes_queue(argv);
  }
  else
    printf("Wrong cmd argument!\n");
  exit(0);
}

void dish_dryer(int argc, char* argv[])
{
  if(!strcmp("sem", argv[2]))
  {
    recieve_dishes_sem_clean(argv);
  }
  else if(!strcmp("queue", argv[2]))
  {
    recieve_dishes_queue_clean(argv);
  }
  else
    printf("Wrong cmd argument!\n");
  exit(0);
}

int sem_get_access(const char* my_pathname)
{
  int semid = 0;
  key_t   key; 

  if((key = ftok(my_pathname , 0)) < 0)
  {
    printf("Can\'t generate key\n");
    exit(1);
  }

  if((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0)
  {
    printf("Can\'t get semid\n");
    exit(-1);
  }

  return semid;
}

void delete_sem(int semid)
{
   if(semctl(semid, 0, IPC_RMID)< 0)
   {
      printf("Can't delete semaphore\n");
      exit(-1);
   }
}

int shmem_get_access(const char* my_pathname, int size)
{
  
   int shmid = 0;
   key_t   key; 

   if((key = ftok(my_pathname , 0)) < 0)
   {
     printf("Can\'t generate key\n");
     exit(1);
   }
  
   if((shmid = shmget(key, size * sizeof(node_t), 0666 | IPC_CREAT | IPC_EXCL)) < 0)
   {        
       if(errno != EEXIST) 
       {  
         printf("Can't create shared memory\n");
         exit(1);
       } 
       else 
       {
           if((shmid = shmget(key, size * sizeof(node_t), 0)) < 0)
           {
             printf("Can't find shared memory\n");
             exit(1);
           }
       }
   }
   
   return shmid;
}

void delete_shmem(int shmid)
{
   if(shmctl(shmid, IPC_RMID, NULL) < 0)
   {
      printf("Can't delete shared memory\n");
      exit(-1);
   }
}

node_t* attach_shmem(int shmid)
{
  node_t* shared_array_pointer = NULL;
      
  if((shared_array_pointer = (node_t*)shmat(shmid, NULL, 0)) == (node_t*)(-1))
  {
     printf("Can't attach shared memory\n");
     exit(-1);
  }
  
   return shared_array_pointer;
}

void detach_shmem(node_t* shared_array_pointer)
{    
   if(shmdt((char*)shared_array_pointer) < 0)
   {
      printf("Can't detach shared memory\n");
      exit(-1);
   }
}

int queue_get_access(const char* my_pathname)
{ 
  int msgid = 0;
  key_t   key; 

  if ((key = ftok(my_pathname, 0)) < 0)
  {
    printf("Can\'t generate key\n");
    exit(-1);
  }

  if((msgid = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    printf("Can\'t get msqid\n");
    exit(-1);
  }
   
   return msgid;
}

void delete_queue(int msgid)
{
   if(msgctl(msgid, IPC_RMID, (struct msqid_ds*)NULL) < 0)
   {
      printf("Can't delete queue\n");
      exit(-1);
   }
}



