#include "http/ctp_http_parse_request.h"

const void parse_headers(CTP_HTTP_REQUEST_HEADERS *headers, char **headersToParse, int headersToParseSize) {
  for (int i = 0; i < headersToParseSize; i++) {
    char *bfr = strdup(headersToParse[i]);
    char *del = strstr(bfr, ": ");

    headers->headers[i].value = strdup(&del[2]);

    bfr[del - bfr] = '\0';
    
    headers->headers[i].key = strdup(bfr);

    headers->headersSize++;

    printf("HEADER: %s: %s\n", headers->headers[i].key, headers->headers[i].value);

    free(bfr);
  }
}


const void ctp_parse_request(const char *request, int requestSize, CTP_HTTP_REQUEST *rq) {
  char **rc_headers;
  char *rc_block;
  int rc_header_size = 0;
  
  char rc_headers_buffer[2048];
  char rc_request_buffer[requestSize];

  int headerMaxSize = CTP_KEY_HEADER_MAX_SIZE + CTP_VALUE_HEADER_MAX_SIZE;
  
  memcpy(rc_request_buffer, request, sizeof(char) * requestSize);

  if(!(rc_headers = malloc(sizeof(char*) * CTP_HEADER_MAX_HEADERS))) {
    printf("Error on alocate resource for rc_headers CHANGEME");
    _exit(-1);
  }
  
  if(!(rc_block = malloc(sizeof(char) * headerMaxSize * CTP_HEADER_MAX_HEADERS))) {
    printf("Error on alocate resource for rc_block CHANGEME");
    free(rc_headers);
    _exit(-1);
  }
  
  for (int i = 0; i < CTP_HEADER_MAX_HEADERS; i++) {
    rc_headers[i] = &rc_block[i * headerMaxSize];
  }

  // GET BASIC REQUEST INFORMATION
  char *rcRequestMethod = strtok(rc_request_buffer, " ");
  char *rcRequestRoute = strtok(NULL, " ");
  char *rcRequestProtocol = strtok(NULL, "\r\n");

  if (!rcRequestMethod || !rcRequestRoute || !rcRequestProtocol) {
    return;
  }

  char *end_token = strstr(request, "\r\n\r\n");
  if (end_token != NULL) {
    memcpy(rc_headers_buffer, request, (size_t) (end_token - request));
  }

  // GET PRE COMPUTED HEADERS
  char *token = strtok(rc_headers_buffer, "\r\n");
  for(int i = 0;; i++) {
    token = strtok(0, "\r\n");
    if(token == NULL || strstr(&token[strlen(token) + 1], "\r\n\r\n") != NULL) break;
    
    sprintf(rc_headers[i], "%s", token);
    rc_header_size++;
  }
  
  printf("\nMethod: '%s'\nRoute: '%s'\nProtocol: '%s'\n", rcRequestMethod, rcRequestRoute, rcRequestProtocol);
  
  rq->route = strdup(rcRequestRoute);

  parse_headers(&rq->headers, rc_headers, rc_header_size);

  printf("Header quantity: %d\n\n", rq->headers.headersSize);

  free(rc_headers);
  free(rc_block);
  
  return;
}

