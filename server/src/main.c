#include "ctp_server.h"
#include <stdio.h>
#include <string.h>
extern int CTP_ERRORNO;

#define PORT 8080

int main(int argc, char **argv) {
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
    char rc_request_method[64];
    char rc_route[128];
    char rc_protocol[64];

    char rc_headers[10240][50];
    int rc_header_size = 0;
    
    char rc_buffer[2048];
    char rc_headers_buffer[2048];
    char rc_request[2048];
  
    if((clientfd = accept(server->socketfd, (struct sockaddr*) server->servaddr, &server->servaddrlen)) < 0) {
      fprintf(stderr, "Error setting up new connection socket");
      return (EXIT_FAILURE);
    }
  
    rc = read(clientfd, rc_buffer, 2048 - 1);
    strcpy(rc_request, rc_buffer);

    // GET BASIC REQUEST INFORMATION
    sprintf(rc_request_method, "%s", strtok(rc_buffer, " "));
    sprintf(rc_route, "%s", strtok(0, " "));
    sprintf(rc_protocol, "%s", strtok(0, "\r\n"));

    char *end_token = strstr(rc_request, "\r\n\r\n");
    if (end_token != NULL) {
      memcpy(rc_headers_buffer, rc_request, (size_t) (end_token - rc_request));
    }

    // GET HEADERS
    char *token = strtok(rc_headers_buffer, "\r\n");
    for(int i = 0;; i++) {
      token = strtok(0, "\r\n");
      if(token == NULL || strstr(&token[strlen(token) + 1], "\r\n\r\n") != NULL) break;
      
      sprintf(rc_headers[i], "%s", token);
      rc_header_size++;
    }    
    
    printf("\nMethod: '%s'\nRoute: '%s'\nProtocol: '%s'\n\n", rc_request_method, rc_route, rc_protocol);
    
    printf("\nHeaders: \n");
    
    for(int i = 0; i < rc_header_size; i++) {
      printf("HEADER: %s\n", rc_headers[i]);
    }
    
    printf("\n\nOriginal send request:\n%s\n\n", rc_request);

    send(clientfd, send_content, strlen(send_content), 0);

    close(clientfd);
  }
  
  close(server->socketfd);

  return 0;
}

