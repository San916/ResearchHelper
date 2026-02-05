#ifndef HTTP_ORCHESTRATOR_PRIV_H
#define HTTP_ORCHESTRATOR_PRIV_H

#include "handlers.h"

#include <stdlib.h>
#include <stdbool.h>

typedef struct HttpRequest HttpRequest;
typedef struct HttpResponse HttpResponse;
typedef struct Route Route;

struct HttpHandle {
    Route* routes;
    size_t num_routes;
    HttpRequest* (*parse_http_request)(const char*, size_t, int*);
    HttpResponse (*handle_parse_http_request_error)(HttpRequest*, int);
    void (*find_route)(Route*, size_t, HttpRequest*, HttpResponse*);
    char* (*build_http_response)(HttpResponse*);
    char* (*handle_build_http_response_error)(HttpResponse*);
};

#endif