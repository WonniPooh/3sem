void send_dishes_in_a_queue(int msgid, mybuf_t *mybuf_output, int size);
void fill_the_table(dish_t* list_state);
void add_new_dirty(dish_t* new_dish);
void end_it(dish_t* new_node);
void generate_dishes_queue();
void generate_dishes_sem();

void generate_dishes_sem()
{
  int shmid = shmem_get_access(pathname, WASHER_ARRAY_SIZE);
   
  srand(time(NULL));
     
  dish_t* first_node = attach_shmem(shmid);
  fill_the_table(first_node);

  printf("Generator semaphore mission complete!\n");
}

void fill_the_table(dish_t* first_node)
{
  dish_t* current_node = first_node;
  int semid = sem_get_access(pathname);   
  struct sembuf there_is_full = {};
  PREPARE_OP(there_is_full , 0, 1, 0);
  
  int dishes_amount = rand() % 10 + 1;

  for(int i = 0; i < dishes_amount; i++)
  {
    if(current_node -> time_to_wash == 0 || current_node -> is_it_clean == 1)
      add_new_dirty(current_node);
   
    semop(semid, &there_is_full, 1);
    
    printf("%s : %d\n", current_node -> dish_name, current_node -> time_to_wash); 
    current_node++; 
  }
  
  end_it(current_node);
  semop(semid, &there_is_full, 1);
}

void end_it(dish_t* new_node)
{
  new_node -> time_to_wash = END_OF_THE_DAY;
}
    
void add_new_dirty(dish_t* new_dish)
{
  int current_dish = rand() % 10;

  strcpy(new_dish -> dish_name, "glass0");	

  FILL_THE_DISH(current_dish, current_dish, current_dish);
  
}

void generate_dishes_queue()
{
  srand(time(NULL));
  
  int dishes_amount = rand() % 10 + 1;
  
  int msgid = queue_get_access(pathname); 

  mybuf_t mybuf_output;
    
  for(int i = 0; i < dishes_amount; i++)
  {    
    add_new_dirty(&mybuf_output.dish_node);
    mybuf_output.mtype = 1;
    send_dishes_in_a_queue(msgid, &mybuf_output, sizeof(dish_t));
    printf("%s : %d\n", mybuf_output.dish_node.dish_name, mybuf_output.dish_node.time_to_wash); 
  }
  
  mybuf_output.mtype = 1;
  mybuf_output.dish_node.time_to_wash = END_OF_THE_DAY; 
  send_dishes_in_a_queue(msgid, &mybuf_output, sizeof(dish_t));
  printf("Generator queue mission complete!\n");
}

void send_dishes_in_a_queue(int msgid, mybuf_t *mybuf_output, int size)
{
  if(msgsnd(msgid, (mybuf_t*) mybuf_output, size, 0) < 0)
  {
    printf("Can\'t send message to queue\n");
    delete_queue(msgid);
    exit(-1);
  }
}
