void send_dish_to_dryer(char* argv[], dish_t dish_output);
void send_dryer_sem(int list_size, dish_t dish_output);
void send_dryer_queue(dish_t dish_output);
void recieve_dishes_queue(char* argv[]);
void recieve_dishes_sem(char* argv[]);
void wash_the_dish(int time_to_wash);

void recieve_dishes_queue(char* argv[])
{
  int msgid = queue_get_access(pathname); 
	  
  mybuf_t mybuf_input;
    
  while(1)
  {  	
    if ((msgrcv(msgid, (mybuf_t*) &mybuf_input, sizeof(dish_t), 1, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      delete_queue(msgid);
      exit(-1);
    }
    else
    {
      if(mybuf_input.dish_node.time_to_wash == END_OF_THE_DAY)
      {
        send_dish_to_dryer(argv, mybuf_input.dish_node);
        
        printf("Queue washing work is done!\n");
        
        break;
      }
      else
      {
        printf("washer %s : %d\n", mybuf_input.dish_node.dish_name, mybuf_input.dish_node.time_to_wash);
        
        wash_the_dish(mybuf_input.dish_node.time_to_wash);
        mybuf_input.dish_node.is_it_clean = 1;
        send_dish_to_dryer(argv, mybuf_input.dish_node);
      }
    }
  }
 
  delete_queue(msgid);
}

void wash_the_dish(int time_to_wash)
{ 
  sleep(time_to_wash);
}

void send_dish_to_dryer(char* argv[], dish_t dish_output)
{  
  if(!strcmp("sem", argv[2]))
  {
    send_dryer_sem(atoi(argv[3]), dish_output);
  }
  else if(!strcmp("queue", argv[2]))
  {
    send_dryer_queue(dish_output);
  }
  else
  {
    printf("Wrong cmd argument!\n");
    exit(0);
  }
  
}

void send_dryer_sem(int list_size, dish_t dish_output)
{ 
  static int is_it_first_time = 0;
  static int msg_sent = 0;
  static dish_t* first_node = NULL;
  static dish_t* current_node = NULL;
 
  int semid_place = sem_get_access(pathname_to_dry);
  int semid_diff = sem_get_access(pathname_to_wash);
  int shmid = shmem_get_access(pathname_to_dry, list_size);
    
  if(is_it_first_time == 0)
  {
    is_it_first_time = 1;
    first_node = attach_shmem(shmid);
    current_node = first_node;
  }
  
  struct sembuf is_there_free = {};
  struct sembuf is_there_full = {};  
  
  PREPARE_OP(is_there_free, 0, -1, 0);
  PREPARE_OP(is_there_full, 0, 1, 0);
  
  semop(semid_place , &is_there_free, 1);

  strcpy( current_node -> dish_name, dish_output.dish_name);		
  current_node -> time_to_dry = dish_output.time_to_dry;		
  current_node -> time_to_wash = dish_output.time_to_wash;
  current_node -> is_it_clean = dish_output.is_it_clean;
  
  semop(semid_diff, &is_there_full, 1);
  msg_sent++;
  current_node = first_node + msg_sent % list_size;
}

void send_dryer_queue(dish_t dish_output)
{
  int semid = sem_get_access(pathname_to_dry);
  
  int msgid = queue_get_access(pathname_to_dry);

  mybuf_t mybuf_output = {};
  mybuf_output.mtype = 2;
  mybuf_output.dish_node = dish_output;

  struct sembuf sending = {};
  
  PREPARE_OP(sending, 0, -1, 0);
  
  semop(semid, &sending, 1);
  send_dishes_in_a_queue(msgid, (mybuf_t*) &mybuf_output, sizeof(dish_t));
} 

void recieve_dishes_sem(char* argv[])
{
  int semid = sem_get_access(pathname);
  int shmid = shmem_get_access(pathname, WASHER_ARRAY_SIZE);
     
  struct sembuf is_there_full = {};

  PREPARE_OP(is_there_full, 0, -1, 0);

  dish_t* current_node = NULL;
  dish_t* first_node = NULL;
     
  current_node = attach_shmem(shmid);
  first_node = current_node;
  
  while(current_node -> time_to_wash != END_OF_THE_DAY)
  {
    semop(semid, &is_there_full, 1);
    
    printf("washer %s : %d\n", current_node -> dish_name, current_node -> time_to_wash);
   
    wash_the_dish(current_node -> time_to_wash);
   
    current_node -> is_it_clean = 1;
        
    send_dish_to_dryer(argv, *current_node);
    current_node++;
  }

  semop(semid, &is_there_full, 1);
  send_dish_to_dryer(argv, *current_node);
  
  printf("Washer Semaphore Mission complete!\n");
  
  detach_shmem(first_node);
  delete_shmem(shmid);
  semctl(semid, 0, IPC_RMID);    
}
