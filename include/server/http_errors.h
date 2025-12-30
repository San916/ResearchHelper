#ifndef HTTP_ERRORS_H
#define HTTP_ERRORS_H

#include "http.h"

#define MINIMAL_500_RESPONSE "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nConnection: close\r\nContent-Length: 21\r\n\r\n500 Internal Error"

HttpResponse handle_400(HttpRequest* req);
HttpResponse handle_404(HttpRequest* req);
HttpResponse handle_500();

#endif