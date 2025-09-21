#ifndef SERVER_H_
#define SERVER_H_

#include "ctp_base_includes.h"

typedef struct ctp_server {
  int socketfd;
  
  int domain;
  int protocol;
  int type;

  struct sockaddr_in *servaddr;
  socklen_t servaddrlen;
}ctp_server, *pctp_server;

extern int CTP_ERRORNO;

// Create socket with setsockopt reuse address 
pctp_server create_server(int domain, int type, int protocol, char *address, int port);

#endif

