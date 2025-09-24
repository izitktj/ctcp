#ifndef CTP_HTTP_H_
#define CTP_HTTP_H_

#include "ctp_base_includes.h"
#include "ctp_http_errornos.h"

#include "ctp_server.h"

#define CTP_HEADER_MAX_SIZE 4096
#define CTP_HEADER_MAX_HEADERS 100

#define CTP_ROUTE_MAX_SIZE 2048
#define CTP_ROUTE_MAX_PARAMS 100

#define CTP_HTTP_METHOD_UNKNOWN 0
#define CTP_HTTP_METHOD_GET     1
#define CTP_HTTP_METHOD_HEAD    2
#define CTP_HTTP_METHOD_POST    3
#define CTP_HTTP_METHOD_PUT     4
#define CTP_HTTP_METHOD_PATCH   5
#define CTP_HTTP_METHOD_DELETE  6
#define CTP_HTTP_METHOD_OPTIONS 7

#define CTP_HTTP_PROTOCOL_UNDEFINED 0
#define CTP_HTTP_PROTOCOL_HTTP_1_1 1
#define CTP_HTTP_PROTOCOL_HTTP_2 2

extern int CTTP_SOCKET_ERRORNO;

typedef struct CTP_HTTP_REQUEST_HEADER{
  char key[CTP_HEADER_MAX_SIZE / 2];
  char value[CTP_HEADER_MAX_SIZE / 2];
}CTP_HTTP_REQUEST_HEADER, *PCTP_HTTP_REQUEST_HEADER;

typedef struct CTP_HTTP_ROUTE_PARAM {
  _Bool isQuery;
  char name[CTP_ROUTE_MAX_SIZE / 2];
  char value[CTP_ROUTE_MAX_SIZE / 2];
}CTP_HTTP_ROUTE_PARAM, *PCTP_HTTP_ROUTE_PARAM;

typedef struct CTP_HTTP_REQUEST{
  CTP_HTTP_REQUEST_HEADER headers[CTP_HEADER_MAX_HEADERS];

  int method;
  int protocol;
  
  char *route;
  CTP_HTTP_ROUTE_PARAM routeParams[CTP_ROUTE_MAX_PARAMS];
}CTP_HTTP_REQUEST, *PCTP_HTTP_REQUEST;

void ctp_listen_requests(ctp_server *server_socket);

PCTP_HTTP_REQUEST ctp_handle_http_request(int clientfd);

static void ctp_parse_response(const char *request, int requestSize, PCTP_HTTP_REQUEST rq);

#endif
