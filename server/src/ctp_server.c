#include "ctp_server.h"

int CTP_ERRORNO = 0;

static void handle_error(unsigned short int error_code, const char *log_statment) {
  CTP_ERRORNO= error_code;
  // Change to a log proper log function
  fprintf(stderr, "%s", log_statment);
}

pctp_server create_server(int domain, int type, int protocol, char *address, int port){
  pctp_server serverfd = (pctp_server) malloc(sizeof(ctp_server));
  struct sockaddr_in *servaddr = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
  socklen_t servaddrlen = sizeof(struct sockaddr_in);
  int opt = 1;

  if(!serverfd) {
    handle_error(1, "Error alocating serverfd: ctp_server.c:15\n");
    return NULL;
  }

  if(!servaddr) {
    handle_error(1, "Error alocating servaddr: ctp_server.c:19\n");
    return NULL;
  }
  
  serverfd->socketfd    = socket(domain, type, protocol);
  serverfd->domain      = domain;
  serverfd->protocol    = protocol;
  serverfd->type        = type;
  serverfd->servaddr    = servaddr;
  serverfd->servaddrlen = servaddrlen;

  memset(servaddr, 0, sizeof(struct sockaddr_in));

  servaddr->sin_addr.s_addr = inet_addr(address);
  servaddr->sin_port        = htons(port);
  servaddr->sin_family      = domain;
  
  if(setsockopt(serverfd->socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    handle_error(2, "Error binding socket on: ctp_server:34\n");
    return NULL;
  }

  if(bind(serverfd->socketfd, (struct sockaddr *) servaddr, sizeof(struct sockaddr_in)) < 0) {
    handle_error(3, "Error binding socket on: ctp_server:38\n");
    return NULL;
  }

  return serverfd;
}

