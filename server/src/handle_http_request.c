#include "ctp_http.h"
#include "ctp_http_errornos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int CTP_ERRORNO;
int CTP_SOCKET_ERORRNO = 0;

static void *handle_error(int errorCode, const char *message) {
  CTP_SOCKET_ERORRNO = errorCode;
  fprintf(stderr, "%s", message);
  return NULL;
}

void ctp_listen_requests(PCTP_SERVER server_socket) {
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
  if (!rq) {
    handle_error(CTP_ERRORNO_ALLOC, "Error while alocating request");
    _exit(-1);
  }
  
  char rqMsgBffr[10240];
  read(clientfd, rqMsgBffr, sizeof(rqMsgBffr) - 1);
  printf("Incoming request:");
  // TODO: Parsear direito a requisição rq
  ctp_parse_request(rqMsgBffr, sizeof(rqMsgBffr), rq);
  ctp_send_response(rq->route, clientfd);
  
  return rq;
}

static void ctp_parse_request(const char *request, int requestSize, PCTP_HTTP_REQUEST rq) {
    char rc_request_method[64];
    char rc_route[128];
    char rc_protocol[64];

    char rc_headers[10240][50];
    int rc_header_size = 0;
    
    char rc_headers_buffer[2048];
    char rc_request_buffer[requestSize];

    memcpy(rc_request_buffer, request, sizeof(char) * requestSize);

    if(!rc_request_buffer[0]) {
      handle_error(CTP_ERRORNO_ALLOC, "Error alocating memory for request buffer");
      _exit(-1);
    }

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

    rq->route = strdup(rc_route);
    
    return;
}


static void ctp_send_response(const char *fileLocation, int clientfd) {
  int fileSize = 0;
  char *fileBuffer = NULL;
  char *header = NULL;
  char *responseBuffer = NULL;
  ssize_t sent;

  if (ctp_read_file(&fileBuffer, &fileSize, fileLocation) != 0) {
    handle_error(CTP_ERRORNO_GET_FILE, "Error getting file to response\n");
    return;
  }

  header = generate_header("HTTP/1.1", "200 OK", "text/html");
  responseBuffer = malloc(strlen(header) + fileSize + 1);
  if (!responseBuffer) {
    handle_error(CTP_ERRORNO_ALLOC, "Error allocating response buffer\n");
    free(header);
    free(fileBuffer);
    return;
  }

  memcpy(responseBuffer, header, strlen(header));
  memcpy(responseBuffer + strlen(header), fileBuffer, fileSize);
  responseBuffer[strlen(header) + fileSize] = '\0';

  sent = send(clientfd, responseBuffer, strlen(header) + fileSize, 0);

  free(header);
  free(fileBuffer);
  free(responseBuffer);
}

char *generate_header(const char *protocol, const char *statusCode, const char *contentType) {
  char *headerBuffer;
  int sizeHeaderBuffer;

  sizeHeaderBuffer =
    strlen(protocol)   + 1 + // +1 to include whitespaces
    strlen(statusCode) + 1 +
    strlen(contentType) + 13 + 1 +
    sizeof(char) * 6; 
    
  headerBuffer = (char *) malloc(sizeHeaderBuffer);

  snprintf(headerBuffer, sizeHeaderBuffer, "%s %s\r\nContent-Type: %s\r\n\r\n", protocol, statusCode, contentType);

  return headerBuffer;
}


static int ctp_read_file(char **file, int *fileSize, const char *fileLocation) {
  FILE *fptr = NULL;
  size_t read_bytes = 0;
  
  char *fileLocationPatch = strdup(&fileLocation[1]);

  fptr = fopen(fileLocationPatch, "rb");
  if (!fptr) {
    fptr = fopen("internal/not_found.html", "rb");
    free(fileLocationPatch);
    if (!fptr) return -1;
  }

  free(fileLocationPatch);

  fseek(fptr, 0L, SEEK_END);

  if ((*fileSize = (int) ftell(fptr)) < 0L) {
    handle_error(CTP_ERRORNO_GET_FILE_SIZE, "Error getting file size\n");
    return -1;
  }

  fseek(fptr, 0L, SEEK_SET);

  if (!(*file = malloc((size_t)(*fileSize) + 1))) {
    fclose(fptr);
    return -1;
  }

  read_bytes = fread(*file, 1, (size_t)*fileSize, fptr);

  (*file)[read_bytes] = '\0';

  fclose(fptr);
  return 0;
}

static void ctp_send_response(const char *fileLocation, int fileLocationSize) {
  FILE *fptr;
  int fileSize;
  char *file_buffer, header;

  if((fptr = (FILE *) fopen(fileLocation, "r")) == NULL) {
    handle_error(CTP_ERRORNO_GET_FILE, "Error getting file to response");
    // Awser a 500 error
    return;
  }

  fseek(fptr, 0L, SEEK_END);
  fileSize = ftell(fptr);

  if((file_buffer = (char *) malloc(sizeof(char) * fileSize)) == NULL) {
    handle_error(CTP_ERRORNO_ALLOC, "Error alocating data for file");
    return;
  }

  fgets(file_buffer, fileSize, fptr);
  size_t read_bytes = fread(file_buffer, 1, sizeof(fileSize) - 1, fptr);
  file_buffer[read_bytes] = '\0';

  header = generate_header("", char *statusCode, char *contentType)

  snprintf(file_buffer, read_bytes + 1, "%s%s", );
}

const char *generate_header(char *protocol, char *statusCode, char *contentType) {
  char *headerBuffer;
  int sizeHeaderBuffer;

  sizeHeaderBuffer =
    sizeof(protocol)   + 1 + // +1 to include whitespaces
    sizeof(statusCode) + 1 +
    sizeof(contentType)+ 1 +
    sizeof(char) * 6; 
    
  headerBuffer = (char *) malloc(sizeHeaderBuffer);

  snprintf(headerBuffer, sizeHeaderBuffer, "%s %s\r\n%s\r\n\r\n", protocol, statusCode, contentType);

  return headerBuffer;
}
