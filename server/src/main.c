#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>

  #include <unistd.h>
  #include <sys/types.h>
#endif

#define PORT 8080

int main(int argc, char **argv) {
  struct sockaddr_in servaddr;
  socklen_t servaddrlen = sizeof(servaddr);
  char *rc_buffer = (char *) malloc(sizeof(char) * 4096);
  char *send_content = (char *) malloc(sizeof(char) * 10240);
  const char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  char file_buffer[8192];
  FILE *fptr;
  int serverfd,opt;

  opt = 1;
  servaddrlen = sizeof(servaddr);

  if((fptr = (FILE *) fopen("index.html", "r")) == NULL) {
    fprintf(stderr, "Error reading index.html!\n");
    return -1;
  }


  fgets(send_content, 10240, fptr);
  size_t read_bytes = fread(file_buffer, 1, sizeof(file_buffer) - 1, fptr);
  file_buffer[read_bytes] = '\0'; // garante finalização

  // Agora monta a resposta completa em send_content
  snprintf(send_content, 10240, "%s%s", header, file_buffer);
  
  ssize_t rc;

  if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Error creating socket");
    return -1;
  }

  if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "Error setting up socket\n");
    return (-1);
  }
  
  memset(&servaddr, '\0', sizeof(servaddr));
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_family = AF_INET;

  if (bind(serverfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "Error binding socket to server!");
    return(EXIT_FAILURE);
  }

  if(listen(serverfd, 3) < 0) {
    fprintf(stderr, "Error while listening");
    return (EXIT_FAILURE);
  }

  while(1) {
    int clientfd;
    if((clientfd = accept(serverfd, (struct sockaddr*) &servaddr, &servaddrlen)) < 0) {
      fprintf(stderr, "Error setting up new connection socket");
      return (EXIT_FAILURE);
    }
  
    rc = read(clientfd, rc_buffer, 1024 - 1);
    printf("%s\n", rc_buffer);

    printf("Sending:\n\n%s\nEnd content\n\n", send_content);

    send(clientfd, send_content, strlen(send_content), 0);

    close(clientfd);
  }
  
  close(serverfd);

  return 0;
}

