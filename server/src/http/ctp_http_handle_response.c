#include "http/ctp_http_handle_response.h"

extern int CTP_ERRORNO;
unsigned int CTP_RESPONSE_ERRORNO = 0;

unsigned int handle_error(int errorCode, const char *message) {
  CTP_RESPONSE_ERRORNO = errorCode;
  fprintf(stderr, "%s", message);
  return errorCode;
}

// void ctp_send_response(const char *fileLocation, int clientfd) {
//   int fileSize = 0;
//   char *fileBuffer = NULL;
//   char *header = NULL;
//   char *responseBuffer = NULL;
//   ssize_t sent;

//   if (ctp_read_file(&fileBuffer, &fileSize, fileLocation) != 0) {
//     handle_error(CTP_ERRORNO_GET_FILE, "Error getting file to response\n");
//     return;
//   }

//   header = generate_header("HTTP/1.1", "200 OK", "text/html");
//   responseBuffer = malloc(strlen(header) + fileSize + 1);
//   if (!responseBuffer) {
//     handle_error(CTP_ERRORNO_ALLOC, "Error allocating response buffer\n");
//     free(header);
//     free(fileBuffer);
//     return;
//   }

//   memcpy(responseBuffer, header, strlen(header));
//   memcpy(responseBuffer + strlen(header), fileBuffer, fileSize);
//   responseBuffer[strlen(header) + fileSize] = '\0';

//   sent = send(clientfd, responseBuffer, strlen(header) + fileSize, 0);

//   free(header);
//   free(fileBuffer);
//   free(responseBuffer);
// }

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


int ctp_read_file(char **file, int *fileSize, const char *fileLocation) {
  FILE *fptr = NULL;
  size_t read_bytes = 0;
  
  char *fileLocationPatch = strdup(&fileLocation[1]);

  fptr = fopen(fileLocationPatch, "rb");
  if (!fptr) {
    fptr = fopen("internal/not_found.html", "rb");
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
