#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"

typedef HttpResponse (*HttpHandler)(HttpRequest*);

typedef struct Route {
    const char* path;
    HttpHandler handler;
} Route;

void find_route(Route* routes, int num_routes, HttpRequest* req, HttpResponse* resp);

#endif