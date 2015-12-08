void dry_the_dish(int time_to_dry);
void recieve_dishes_queue_clean(char* argv[]);
void recieve_dishes_sem_clean();
int sem_ini(int value);

void recieve_dishes_sem_clean(char* argv[])
{
  int table_size = atoi(argv[3]);
  int semid_place = sem_ini(table_size);
  int semid_diff = sem_get_access(pathname_to_wash);
  int shmid = shmem_get_access(pathname_to_dry, atoi(argv[3]));
  
  dish_t* current_node = attach_shmem(shmid);
  dish_t* first_node = current_node;

  struct sembuf is_there_free = {};
  struct sembuf is_there_full = {};  
  
  PREPARE_OP(is_there_free, 0, 1, 0);
  PREPARE_OP(is_there_full, 0, -1, 0);

  semop(semid_diff, &is_there_full, 1);
  
  int nodes_amount = 0;
  
  while(current_node -> time_to_wash != END_OF_THE_DAY)
  {      
    if(nodes_amount != 0)
       semop(semid_diff, &is_there_full, 1);
    
    nodes_amount++;    
    printf("dryer %s : %d\n", current_node -> dish_name, current_node -> time_to_dry);
        
    dry_the_dish(current_node -> time_to_dry);
    current_node -> is_it_dry = 1;
    current_node = first_node + nodes_amount % table_size;
    semop(semid_place , &is_there_free, 1);
  }
  
  
  /*while(current_node -> dish.time_to_wash != END_OF_THE_DAY)
  {
    if(nodes_amount++ != 0) 
      semop(semid_diff, &is_there_full, 1);
    
    printf("dryer %s : %d\n", current_node -> dish.dish_name, current_node -> dish.time_to_dry);
   
    dry_the_dish(current_node -> dish.time_to_dry);
	current_node -> dish.is_it_dry = 1;
    
    semop(semid_place , &is_there_free, 1);
        
   // current_node = current_node -> next;
  }
*/
  printf("Dryer Mission complete!\n");
  
  detach_shmem(first_node);
  delete_shmem(shmid);
  semctl(semid_place, 0, IPC_RMID); 
  semctl(semid_diff, 0, IPC_RMID); 
}

void recieve_dishes_queue_clean(char* argv[])
{
  printf("Hello I am queue dryer\n");
  int semid = sem_ini(atoi(argv[3]));
  int msgid = queue_get_access(pathname_to_dry); 
   
  struct sembuf washed = {};
  PREPARE_OP(washed, 0, 1, 0);   
     
  mybuf_t mybuf_input;
    
  while(1)
  
  {  	
    if ((msgrcv(msgid, (mybuf_t *) &mybuf_input, sizeof(dish_t), 2, 0)) < 0)
    {
      printf("Can\'t receive message from queue\n");
      delete_queue(msgid);
      exit(-1);
    }
    else
    {
      if(mybuf_input.dish_node.time_to_wash == END_OF_THE_DAY)
      {
        delete_sem(semid);
        delete_queue(msgid);
        printf("Queue drying work is done!\n");
        break;
      }
      else
      {
        printf("dryer %s : %d\n", mybuf_input.dish_node.dish_name, mybuf_input.dish_node.time_to_dry);
        dry_the_dish(mybuf_input.dish_node.time_to_dry);
      
        semop(semid, &washed, 1);         
      }
    }
  } 
}

void dry_the_dish(int time_to_dry)
{ 
  sleep(time_to_dry);
}

int sem_ini(int value)
{
  int semid = sem_get_access(pathname_to_dry);
  struct sembuf init = {};
      
  init.sem_num = 0;				
  init.sem_op =  value;												
  init.sem_flg = 0;
  
  semop(semid, &init, 1);

  return semid;
}
