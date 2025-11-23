#include "helpers/ctp_http_helpers.h"

void to_lower(char *str) {
  for(int i = 0; i < strlen(str); i++) {
    if((int) str[i] > 64 && (int) str[i] < 91) {
      str[i] = (char) str[i] + 32;
    }
  }
}

int ctp_get_header(CTP_HTTP_REQUEST_HEADERS *headers, const char* key, char **value){
  char *kkey = strdup(key);
  to_lower(kkey);

  for(int i = 0; i < headers->headersSize;i++) {
    char *toCompare = strdup(headers->headers[i].key);
    to_lower(toCompare);

    if (strcmp(kkey, toCompare) == 0) {
      *value = strdup(headers->headers[i].value);
      free(kkey);
      free(toCompare);
      return i;
    }

    free(toCompare);
  }

  free(kkey);
  return -1;
}
