#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
  #include <stdio.h>
  #include <unistd.h>
#endif

#define PORT 8080

int main(int argc, char **argv) {
  struct sockaddr_in servaddr;
  socklen_t servaddrlen = sizeof(servaddr);
  int newSocket = -1;
  int opt = 1;

  char *rcbuffer = (char *) malloc(sizeof(char) * 1024);
  ssize_t rc;

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0) {
    fprintf(stderr, "Error creating socket");
    return -1;
  }

  char* msgbuffer = (char *) malloc(sizeof(char) * 1024);
  memcpy(msgbuffer, "HELLO FROM TCP SOCKET!\0", 23);
  
  memset(&servaddr, '\0', sizeof(servaddr));

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    fprintf(stderr, "Error setting up socket");
    return (-1);
  }
  
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_family = AF_INET;

  if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "Error binding socket to server!");
    return(EXIT_FAILURE);
  }

  if(listen(sockfd, 3) < 0) {
    fprintf(stderr, "Error while listening");
    return (EXIT_FAILURE);
  }
  
  if((newSocket = accept(sockfd, (struct sockaddr*) &servaddr, &servaddrlen)) < 0) {
    fprintf(stderr, "Error setting up new connection socket");
    return (EXIT_FAILURE);
  }
  
  rc = read(newSocket, rcbuffer, 1024 - 1);
  printf("%s\n", rcbuffer);

  send(newSocket, msgbuffer, strlen(msgbuffer), 0);

  close(newSocket);
  close(sockfd);

  return 0;
}

