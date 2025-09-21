#include "ctp_server.h"
#include "ctp_http.h"

extern int CTP_ERRORNO;
int CTP_SOCKET_ERORRNO = 0;

void handle_http_request(ctp_server *server_socket) {
  int clientfd;
  if((clientfd = accept(server_socket->socketfd, (struct sockaddr*) server_socket->servaddr, &server_socket->servaddrlen)) < 0) {
    fprintf(stderr, "Error setting up new client connection socket\n");
    CTP_SOCKET_ERORRNO = -1;
  }
}
