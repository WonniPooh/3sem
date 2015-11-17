const int END_OF_THE_DAY = 100500;

void send_dishes_in_a_queue(int msgid, mybuf_t *mybuf_output, int size);
void create_list(list_t* list_state, int list_size);
void fill_the_table(list_t* list_state);
void add_new_dirty(node_t* new_node);
void end_it(node_t* new_node);
void generate_dishes_queue();
void generate_dishes_sem();

void generate_dishes_sem()
{
  int shmid = shmem_get_access(pathname, WASHER_ARRAY_SIZE);
   
  srand(time(NULL));
 
  list_t* list_state = (list_t*)calloc(1, sizeof(list_t));
    
  list_state -> first = attach_shmem(shmid);
  create_list(list_state, WASHER_ARRAY_SIZE);
  fill_the_table(list_state);

  printf("Generator semaphore mission complete!\n");
  
}

void create_list(list_t* list_state, int list_size)
{
	node_t* current_node = list_state -> first;
	list_state -> last = &current_node[list_size - 1];
	list_state -> first_data = &current_node[0];
	list_state -> last_data = &current_node[0];								//empty list

	for(int i = 0; i < list_size; i++)
	{	
		if(i != list_size - 1)
			current_node -> next = current_node + 1;
		else
			current_node -> next = list_state -> first;
	
		if(i != 0)
			current_node -> prev = current_node - 1;
		else
			current_node -> prev = &current_node[list_size - 1];
		
		current_node = current_node + 1;			
	}
	
	current_node = list_state -> first;
  
 /* for(int i = 0; i < list_size; i++)
	{
		printf("%d I am %p;\t Prev next = %p;\t Next prev = %p\n", i, current_node, current_node -> next -> prev, current_node -> prev -> next);
		current_node = current_node -> next;
	}
  */
}

void fill_the_table(list_t* list_state)
{
  int semid = sem_get_access(pathname);   
  struct sembuf there_is_full = {};
  PREPARE_OP(there_is_full , 0, 1, 0);
  
  int dishes_amount = rand() % 10 + 1;

  if(WASHER_ARRAY_SIZE - list_state -> elem_count >= dishes_amount + 1)
  {
    list_state -> elem_count += dishes_amount;
    
    for(int i = 0; i < list_state -> elem_count; i++)
    {
      if(list_state -> last_data -> dish.time_to_wash == 0 || list_state -> last_data -> dish.is_it_clean == 1)
        add_new_dirty(list_state -> last_data);
     
      semop(semid, &there_is_full, 1);
      
      printf("%s : %d\n", list_state -> last_data -> dish.dish_name, list_state -> last_data -> dish.time_to_wash); 
     
      list_state -> last_data = list_state -> last_data -> next; 
    }
        
    list_state -> elem_count ++;
    end_it(list_state -> last_data);
    semop(semid, &there_is_full, 1);
    list_state -> last_data = list_state -> last_data -> next; 
  }
  else
  {
    printf("No enough place\n");
  }

}

void end_it(node_t* new_node)
{
  new_node -> dish.time_to_wash = END_OF_THE_DAY;
}
    
void add_new_dirty(node_t* new_node)
{
  int current_dish = rand() % 10 + 1;

  /*
   * FIXIT:
   * Вы 10 раз написали почти один и тот же кусок кода.
   * http://stackoverflow.com/questions/9655202/how-to-convert-integer-to-string-in-c
   * +
   * strcat
   */
  
  switch(current_dish)
  {
    case 1:  
    
    strcpy(new_node -> dish.dish_name, "glass1");		
    new_node -> dish.time_to_dry = 1;		
    new_node -> dish.time_to_wash = 1;					  
    
    break;
    
    case 2:  
    
    strcpy(new_node -> dish.dish_name, "glass2");
    new_node -> dish.time_to_dry = 2;		
    new_node -> dish.time_to_wash = 2;					  
    
    break;
    
    case 3:  
    
    strcpy(new_node -> dish.dish_name, "glass3");
    new_node -> dish.time_to_dry = 3;		
    new_node -> dish.time_to_wash = 3;					  
    
    break;
    
    case 4:  
    
    strcpy(new_node -> dish.dish_name, "glass4");		
    new_node -> dish.time_to_dry = 4;		
    new_node -> dish.time_to_wash = 4;							  
    
    break;
    
    case 5:  
    
    strcpy(new_node -> dish.dish_name, "glass5");	
    new_node -> dish.time_to_dry = 5;		
    new_node -> dish.time_to_wash = 5;						  
    
    break;
    
    case 6:  
    
    strcpy(new_node -> dish.dish_name, "glass6");	
    new_node -> dish.time_to_dry = 6;		
    new_node -> dish.time_to_wash = 6;						  
    
    break;
    
    case 7:  
    
    strcpy(new_node -> dish.dish_name, "glass7");	
    new_node -> dish.time_to_dry = 7;		
    new_node -> dish.time_to_wash = 7;						  
    
    break;
    
    case 8:  
    
    strcpy(new_node -> dish.dish_name, "glass8");	
    new_node -> dish.time_to_dry = 8;		
    new_node -> dish.time_to_wash = 8;						  
    
    break;
        
    case 9:  
    
    strcpy(new_node -> dish.dish_name, "glass9");		
    new_node -> dish.time_to_dry = 9;		
    new_node -> dish.time_to_wash = 9;							  
    
    break;
    
    case 10:  
    
    strcpy(new_node -> dish.dish_name, "glass10");	
    new_node -> dish.time_to_dry = 10;		
    new_node -> dish.time_to_wash = 10;							  
    
    break;
    
  }
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
    send_dishes_in_a_queue(msgid, &mybuf_output, sizeof(node_t));
    printf("%s : %d\n", mybuf_output.dish_node.dish.dish_name, mybuf_output.dish_node.dish.time_to_wash); 
  }
  
  mybuf_output.mtype = 1;
  mybuf_output.dish_node.dish.time_to_wash = END_OF_THE_DAY; 
  send_dishes_in_a_queue(msgid, &mybuf_output, sizeof(node_t));
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

