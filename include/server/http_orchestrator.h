#ifndef HTTP_ORCHESTRATOR_H
#define HTTP_ORCHESTRATOR_H

#include "http.h"
#include "router.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Route Route;
typedef struct HttpHandle HttpHandle;

HttpHandle* get_http_handle();
void destroy_http_handle(HttpHandle* handle);
void set_http_handle_routes(HttpHandle* handle, Route* routes, size_t num_routes);
char* handle_request(HttpHandle* handle, const char* buffer, size_t bytes, bool* keep_alive);

#endif