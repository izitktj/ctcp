#include "http/ctp_http_mimes.h"

void ctp_get_mime(char *file, char **output) {
  char *fileExtention = &strstr(file, ".")[1];

  if (strcmp(fileExtention, "css") == 0){
    *output = strdup("text/css");
  } else if (strcmp(fileExtention, "png") == 0){
    *output = strdup("image/png");
  } else {
    *output = strdup("text/html");
  }
}