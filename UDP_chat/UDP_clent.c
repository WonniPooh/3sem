#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define EXIT_SEQ ":quit"
#define MAX_NICK_LEN 100
#define MAX_MSG_LEN 1000
#define SERVER_PORT 53000

int main(int argc, char** argv[])
{
  int socket_fd;
  int msg_size_recieved = 0;
  char sendmsg[MAX_MSG_LEN] = {};
  char recvmsg[MAX_MSG_LEN] = {};
  struct sockaddr_in serv_addr = {};
  struct sockaddr_in client_addr = {};

  if (argc != 2)
  {
    printf("Usage: ./a.out <server ip adress> \n");
    exit(1);
  }
  
  if ((socket_fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror(NULL);
    exit(1);
  }

  bzero(&client_addr, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(0);
  client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(socket_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0)
  {
    perror(NULL);
    close(socket_fd);
    exit(1);
  }
  
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  
  if (inet_aton(argv[1], &serv_addr.sin_addr) == 0)
  {
    printf("Invalid IP address\n");
    close(socket_fd);
    exit(1);
  }
  printf("Choose a nickname please:\n");

  fgets(sendmsg, MAX_NICK_LEN, stdin);
  sendmsg[strlen(sendmsg) - 1 ] = '\0';
  
  if (sendto(socket_fd, sendmsg, strlen(sendmsg) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror(NULL);
    close(socket_fd);
    exit(1);
  }

  pid_t pid = fork();

  if(pid != 0)
  {
    while(strcmp(EXIT_SEQ, sendmsg))
    {
      fgets(sendmsg, MAX_MSG_LEN, stdin);

      if (sendto(socket_fd, sendmsg, strlen(sendmsg) + 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
      {
        perror(NULL);
        close(socket_fd);
        exit(1);
      }
    }
  }
  else
  {
    while(1)
    {
      if((msg_size_recieved = recvfrom(socket_fd, recvmsg, MAX_MSG_LEN, 0, (struct sockaddr*) NULL, NULL)) < 0)
      {
        perror(NULL);
        close(socket_fd);
        exit(0);
      }
      
      if(!strcmp(EXIT_SEQ, recvmsg))
      {
        exit(0);
      }
      
      printf("%s\n", recvmsg);
      
      bzero(recvmsg, msg_size_recieved);
    }
  }

  close(socket_fd);
  wait(NULL);
  return 0;
}
