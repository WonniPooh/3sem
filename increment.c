#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

int a = 0;
/*
 * Что значит слово static в этом контексте?
 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* my_thread(void* dummy) {

    pthread_mutex_lock(&mutex);

    /*
     * FIXIT: Надо вынести 1000000 в отдельную константу.
     */
    
    for(int i = 0; i < 1000000; i++)
        a += 1;

    pthread_mutex_unlock(&mutex);
  
    /*
     * Фактически сейчас у вас аналог последовательного кода, что с формальной точки зрения подходит под условие задачи, но всё же.
     * Можно было бы написать так:
     * for(int i = 0; i < 1000000; i++)
     * {
     *  pthread_mutex_lock(&mutex);
        a += 1;
	pthread_mutex_unlock(&mutex);
       }
       чтобы мы ограничили доступ только к критической секции.
     */
    
    return NULL;
}

int main()
{
    pthread_t thread_id1, thread_id2, my_thread_id;

    sem_t *sem = NULL;    

   int  result = pthread_create(&thread_id1 , 
                            (pthread_attr_t *)NULL , 
                            my_thread ,
                            NULL);

    if (result) {
        printf("Can`t create thread, returned value = %d\n" , result);
        exit(-1);
    }

    result = pthread_create(&thread_id2 , 
                            (pthread_attr_t *)NULL , 
                            my_thread ,
                            NULL);

    if (result) {
        printf("Can`t create thread, returned value = %d\n" , result);
        exit(-1);
    }

    my_thread_id = pthread_self();
 
    pthread_join(thread_id1 , (void **) NULL);
    pthread_join(thread_id2 , (void **) NULL);

    printf("a is %d\n", a);

    return 0;
}





































