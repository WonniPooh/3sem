#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREPARE_OP(SEM, NUM, OP, FLAG) SEM.sem_num = (NUM);\
SEM.sem_op = (OP);\
SEM.sem_flg = (FLAG);
 
#define MAX_MSG_SIZE 512
#define SERVER_PORT  53000 
#define MAX_QUEUE_SIZE  2 
#define LOCAL_HOST "127.0.0.1"
#define EXIT_SEQ "exit"
#define KILL_ALL "KILL_THIS"
#define GOODBYE_MSG "User has left the chat\n"
#define INRODUCE_NEW_USER " connected"
#define USERS_NUM 2

const char my_pathname[] = "TCP_server.c";

int sem_get_access(const char* my_pathname);
void delete_sem(int semid);

typedef struct USERS_T
{
  char nickname[MAX_MSG_SIZE];
  int client_socket;
}users_t;
 
int main()
{
  int socket_fd, listener;  
  char msg_send[MAX_MSG_SIZE] = {};
  int msg_counter = 0;
  int users_connected = 0;     
  struct sockaddr_in addr; 
  char recv_msg[MAX_MSG_SIZE] = {};
  users_t users[USERS_NUM] = {};
  int bytes_read; 
  
  int semid = sem_get_access(my_pathname);
  struct sembuf is_there_free_place = {};
  PREPARE_OP(is_there_free_place, 0, -1, 0)

  if((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }
 
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
  
  if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(2);
  }

  listen(listener, MAX_QUEUE_SIZE);
    
  while(1)
  {
    bzero(users[0].nickname, sizeof(char) * strlen(users[0].nickname));
    bzero(users[1].nickname, sizeof(char) * strlen(users[1].nickname));
    
    if((users[0].client_socket = accept(listener, NULL, NULL)) < 0)
		{
			perror("Прием входящих подключений: пользователь 1 подключён");
			exit(3);
		}
        
    if((users[1].client_socket = accept(listener, NULL, NULL)) < 0)
		{
			perror("Прием входящих подключений: пользователь 2 подключён");
			exit(3);
		}
    
    int i = 0;

    /*
     *  FIXIT:
     *  1. У меня tcp чат не работает. В проблеме не разбирался.
     *  2. Участников должно быть N, а не 2. 
     *  3. Я не знаю, как вы с помощью процессов реализуете работу сервера с несколькими процессами, т.к. вы создаете временный 
     *  сокет, который попадает в таблицу файловых дескрипторов для текущего ребёнка. Потом появляется второй клиент. Как о нём узнает 
     *  первый, ведь это разные процессы с разными таблицами файловых дескрипторов.
     */
    for(i = 0; i < 2; i++)
    {
      switch(fork())
      {
        case -1:
          perror("fork");            break;

        case 0:
          close(listener);   
     
        while(1)
        {
          printf("Ожидаем сообщение...\n");
          
          bytes_read = recv(users[i].client_socket, recv_msg, MAX_MSG_SIZE, 0); 
            
          if(bytes_read <= 0) break;
          
          printf("Получено %d bytes\tСообщение: %s\n", bytes_read, recv_msg);
          printf("Отправляю принятое сообщение клиенту\n");
          
          if(!strcmp(KILL_ALL, recv_msg))
          {
            send(users[i].client_socket, EXIT_SEQ, strlen(EXIT_SEQ), 0);
            send(users[!i].client_socket, GOODBYE_MSG, strlen(GOODBYE_MSG), 0);
            PREPARE_OP(is_there_free_place, 0, 1, 0)
            semop(semid, &is_there_free_place, 1);
            close(users[i].client_socket);
            _exit(0);
          }
          
          if(!strcmp(EXIT_SEQ, recv_msg))
          {
            send(users[i].client_socket, EXIT_SEQ, strlen(EXIT_SEQ), 0);
            close(users[i].client_socket);
            _exit(0);
          }
          
          if(!msg_counter)
          {
            strcpy(users[i].nickname, recv_msg);
            strcat(users[i].nickname, ": ");
            strcat(recv_msg, INRODUCE_NEW_USER);
            send(users[!i].client_socket, recv_msg, strlen(recv_msg), 0);
            msg_counter++;
          }
          else
          {
            bzero(msg_send, sizeof(char) * strlen(msg_send));
            strcpy(msg_send, users[i].nickname);
            strcpy((msg_send + strlen(users[i].nickname)), recv_msg);
            send(users[!i].client_socket, msg_send, strlen(msg_send), 0);
          }
        }
      }   
    }
    semop(semid, &is_there_free_place, 1);
  }
    
    return 0;
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

