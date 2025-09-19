#include "ctp_server.h"
extern int CTP_ERRORNO;

#define PORT 8080

int main(int argc, char **argv) {
  char *rc_buffer = (char *) malloc(sizeof(char) * 4096);
  char *send_content = (char *) malloc(sizeof(char) * 10240);
  const char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  char file_buffer[8192];
  FILE *fptr;
  pctp_server server;

  if((fptr = (FILE *) fopen("index.html", "r")) == NULL) {
    fprintf(stderr, "Error reading index.html!\n");
    return -1;
  }

  fgets(send_content, 10240, fptr);
  size_t read_bytes = fread(file_buffer, 1, sizeof(file_buffer) - 1, fptr);
  file_buffer[read_bytes] = '\0';
  
  snprintf(send_content, 10240, "%s%s", header, file_buffer);
  
  ssize_t rc;

  server = create_server(AF_INET, SOCK_STREAM, 0, "127.0.0.1", PORT);
  
  if(listen(server->socketfd, 3) < 0) {
    fprintf(stderr, "Error while listening");
    return (EXIT_FAILURE);
  }

  while(1) {
    int clientfd;
    if((clientfd = accept(server->socketfd, (struct sockaddr*) server->servaddr, &server->servaddrlen)) < 0) {
      fprintf(stderr, "Error setting up new connection socket");
      return (EXIT_FAILURE);
    }
  
    rc = read(clientfd, rc_buffer, 1024 - 1);
    printf("%s\n", rc_buffer);

    printf("Sending:\n\n%s\nEnd content\n\n", send_content);

    send(clientfd, send_content, strlen(send_content), 0);

    close(clientfd);
  }
  
  close(server->socketfd);

  return 0;
}

