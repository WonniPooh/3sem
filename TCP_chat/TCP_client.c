#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
 
#define MAX_MSG_SIZE 512
#define SERVER_PORT  53000
#define EXIT_SEQ "exit"
#define KILL_ALL "KILL_THIS"
 
int main (int argc, char **argv)
{
  char message[MAX_MSG_SIZE] = {}; 
  char recv_msg[MAX_MSG_SIZE] = {};
  int ch = 0;

  if(argc != 2)
  {
    printf("Не верное кол-во аргументов!\nДолжен быть 1 аргумент (ip-адрес сервера)!\n");
    exit(0);
  }
 
  int socket_fd;                 
  struct sockaddr_in serv_addr = {}; 

  if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
      perror("socket");
      exit(1);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT); 
  serv_addr.sin_addr.s_addr=inet_addr(argv[1]);

  if(connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Подключение");
    exit(2);
  }
 
  printf("Enter please your nickname:\n");
 
  pid_t pid = fork();
  
  while(1)
  {
    if(pid != 0)
    {
      gets(message);
    
      send(socket_fd, message, sizeof(message), 0); 
      
      if (!strcmp(message, EXIT_SEQ) || !strcmp(message, KILL_ALL))
      {
        close(socket_fd);
        return 0;
      }
    }
    else
    {
      int bytes_read = 0;

      bytes_read = recv(socket_fd, recv_msg, sizeof(message), 0);
      
      if(!strcmp(EXIT_SEQ, recv_msg))
        _exit(0);
      
      recv_msg[bytes_read] = '\0';
        
      printf("%s\n", recv_msg);
    }  
   
  }
  return 0;
    
}
