#ifndef CTP_HTTP_H_
#define CTP_HTTP_H_

#include "ctp_base_includes.h"
#include "http/ctp_http_errornos.h"

#include "ctp_server.h"

#define CTP_KEY_HEADER_MAX_SIZE   1024
#define CTP_VALUE_HEADER_MAX_SIZE 1024
#define CTP_HEADER_MAX_HEADERS    128

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

typedef struct CTP_HTTP_HEADER{
  char *key;
  char *value;
}CTP_HTTP_HEADER;

typedef struct CTP_HTTP_REQUEST_HEADERS {
  CTP_HTTP_HEADER *headers;

  int headersSize;
} CTP_HTTP_REQUEST_HEADERS;


typedef struct CTP_HTTP_REQUEST{
  CTP_HTTP_REQUEST_HEADERS headers;

  int method;
  int protocol;
  
  char *route;
}CTP_HTTP_REQUEST;

void ctp_listen_requests(CTP_SERVER *server_socket);

CTP_HTTP_REQUEST *ctp_handle_http_request(int clientfd);

void ctp_send_response(const char *file, int clientfd, CTP_HTTP_REQUEST *rq);

#endif
