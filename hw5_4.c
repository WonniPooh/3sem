#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct THREAD_TO_DO
{
  int thread_count;
  int thread_num;
  int matrix_size;
  int* matrix;
  int* output_matrix;
} thread_t;

void* my_thread(void* dummy) 
{
    thread_t* input = (thread_t*)dummy; 
    
    int thread_count =  input -> thread_count;
    int result = 0;
    int thread_num = input -> thread_num;
    int matrix_size = input -> matrix_size;
    int elem_to_start = matrix_size*matrix_size / thread_count * thread_num;
    int elems_to_serve = matrix_size*matrix_size / thread_count;
    int loop_counter = 0;
    int current_y = 0;
    int current_x = 0;
    int i = 0;
     
    while( (matrix_size * matrix_size > elem_to_start) && ((loop_counter < elems_to_serve) || thread_num == thread_count - 1))
    { 
        current_y = elem_to_start / matrix_size;
        current_x = elem_to_start - current_y*matrix_size;
        result = 0;

        for(i = 0; i < matrix_size; i++)
        {
	    result += input -> matrix[current_y * matrix_size + i] * input -> matrix[i * matrix_size + current_x];
        }      
    
        input -> output_matrix[current_y * matrix_size + current_x] = result;
        elem_to_start++;   
        loop_counter++;              
    }
    return NULL;
}

int main()
{
    int matrix_size = 0;
    int thread_count = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    printf("Введите размер матрицы NxN\n");
    scanf("%d", &matrix_size);
    printf("Введите количество потоков\n");
    scanf("%d", &thread_count);

    pthread_t* thread_id_arr = (pthread_t*)calloc(thread_count, sizeof(pthread_t));

    srand ( time(NULL) ); 
    int result;

    int* matrix = (int*)calloc(matrix_size*matrix_size, sizeof(int)); 
    int* multipiled_matrix = (int*)calloc(matrix_size*matrix_size, sizeof(int));  

//create and feel the matrix

    for(j = 0; j < matrix_size; j++)
    {
         for(k = 0; k < matrix_size; k++)
            {
                matrix[j*matrix_size + k] = rand() % 2 + 1;
            }
    }

//print the matrix

    for(i = 0; i < matrix_size; i++)
    {
        for(j = 0; j < matrix_size; j++)
        {
           printf("%d\t", matrix[i*matrix_size + j]);
        }
        printf("\n");
    }
//
   thread_t* thread_info_array = (thread_t*)calloc(thread_count, sizeof(thread_t)); 
   
   for(i = 0; i < thread_count; i++)
   {
      thread_info_array[i].output_matrix = multipiled_matrix;
      thread_info_array[i].thread_num = i;
      thread_info_array[i].thread_count = thread_count;
      thread_info_array[i].matrix_size = matrix_size;
      thread_info_array[i].matrix = matrix;

      result = pthread_create(thread_id_arr + i, (pthread_attr_t *)NULL, my_thread, thread_info_array + i);

      if (result) 
      {
          printf("Can`t create thread, returned value = %d\n" , result);
          exit(-1);
      }
    }
    
    for(i = 0; i < thread_count; i++)
    {
        pthread_join(thread_id_arr[i] , (void **) NULL);
    }

    for(i = 0; i < matrix_size; i++)
    {
        for(j = 0; j < matrix_size; j++)
        {
           printf("%d ", multipiled_matrix[i*matrix_size + j]);
        }
       printf("\n");
    }

    return 0;
}

