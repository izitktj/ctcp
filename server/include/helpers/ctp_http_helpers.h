#ifndef CTP_HTTP_HELPERS_H_
#define CTP_HTTP_HELPERS_H_

#include "http/ctp_http.h"

int ctp_get_header(CTP_HTTP_REQUEST_HEADERS *headers, const char* key, char **value);

#endif
