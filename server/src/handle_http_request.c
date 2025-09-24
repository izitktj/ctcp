#include "ctp_http.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int CTP_ERRORNO;
int CTP_SOCKET_ERORRNO = 0;

static void *handle_error(int errorCode, const char *message) {
  CTP_SOCKET_ERORRNO = errorCode;
  fprintf(stderr, "%s", message);
  return NULL;
}

void ctp_listen_requests(ctp_server *server_socket) {
  int clientfd;
  pid_t cpid;
  PCTP_HTTP_REQUEST rq;
  
  if((clientfd = accept(server_socket->socketfd, (struct sockaddr*) server_socket->servaddr, &server_socket->servaddrlen)) < 0) {
    close(clientfd);
    handle_error(CTP_ERRORNO_SOCKET, "Error accepting client connection\n");
  }
  
  if((cpid = fork()) < 0) {
    handle_error(CTP_ERRORNO_FORK, "Error on fork\n");
  }
 
  if(cpid == 0) {
    rq = ctp_handle_http_request(clientfd);
    _exit(EXIT_SUCCESS);
  }else {
    close(clientfd);
  }
}

PCTP_HTTP_REQUEST ctp_handle_http_request(int clientfd) {
  PCTP_HTTP_REQUEST rq = (PCTP_HTTP_REQUEST) malloc(sizeof(CTP_HTTP_REQUEST));

  char rqMsgBffr[10240];
  read(clientfd, rqMsgBffr, sizeof(rqMsgBffr) - 1);
  ctp_parse_response(rqMsgBffr, sizeof(rqMsgBffr), rq);

  return rq;
}

static void ctp_parse_response(const char *request, int requestSize, PCTP_HTTP_REQUEST rq) {
    char rc_request_method[64];
    char rc_route[128];
    char rc_protocol[64];

    char rc_headers[10240][50];
    int rc_header_size = 0;
    
    char rc_headers_buffer[2048];
    char rc_request_buffer[requestSize];
    
    memcpy(rc_request_buffer, request, sizeof(char) * requestSize);

    // GET BASIC REQUEST INFORMATION
    sprintf(rc_request_method, "%s", strtok(rc_request_buffer, " "));
    sprintf(rc_route, "%s", strtok(0, " "));
    sprintf(rc_protocol, "%s", strtok(0, "\r\n"));

    char *end_token = strstr(request, "\r\n\r\n");
    if (end_token != NULL) {
      memcpy(rc_headers_buffer, request, (size_t) (end_token - request));
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
    
    printf("\n\nOriginal send request:\n%s\n\n", request);

    // send(clientfd, send_content, strlen(send_content), 0);
}
