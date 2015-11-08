#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

int a = 0;
const int cycle_duration = 1000000;
/*
 * Что значит слово static в этом контексте?
 */
 
 //Сейчас задумался, не знаю. Скопировал с сайта на котором читал про семафоры.
 
 /*
  * V: http://stackoverflow.com/questions/572547/what-does-static-mean-in-a-c-program
  * В вашем случае 
  * Static global variables are not visible outside of the C file they are defined in.
  */
 
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* my_thread(void* dummy) {

    pthread_mutex_lock(&mutex);
    
    for(int i = 0; i < cycle_duration; i++)
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
     
     /*Это понятно, спасибо, просто подумал что вызывать lock/unlock столько раз не будет эффективным.*/
    
     /*
      * V: По поводу производительности вы правы. Написал комментарий, чтобы лишний раз обратили внимание.
      * Засчитано.
      * На соответствующем занятии мы также делали упражнение, когда в разделяемую память программа пишет свой код, а вторая
      * его оттуда считывает и выводит на экран. Его также необходимо сделать.
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





































