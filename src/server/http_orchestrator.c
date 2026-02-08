#include "http_orchestrator.h"
#include "http_orchestrator_priv.h"
#include "http.h"
#include "http_errors.h"
#include "handlers.h"
#include "router.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// EFFECTS: Returns handle containg all functions relevant for http request/responses
HttpHandle* get_http_handle() {
    HttpHandle* handle = malloc(sizeof(HttpHandle));

    handle->parse_http_request = parse_http_request;
    handle->handle_parse_http_request_error = handle_parse_http_request_error;
    handle->find_route = find_route;
    handle->build_http_response = build_http_response;
    handle->handle_build_http_response_error = handle_build_http_response_error;

    return handle;
}

// MODIFIES: Cleans up HttpHandle*
void destroy_http_handle(HttpHandle* handle) {
    if (!handle) {
        return;
    }
    free(handle);
}

// REQUIRES: routes should not be made via malloc
// MODIFIES: sets handle->routes and num_routes
void set_http_handle_routes(HttpHandle* handle, Route* routes, size_t num_routes) {
    handle->routes = routes;
    handle->num_routes = num_routes;
}

// REQUIRES: HttpHandle, request buffer, keep alive reference
// MODIFIES: keep_alive
// EFFECTS: Uses handle to process request and return response, sets keep_alive
char* handle_request(HttpHandle* handle, const char* buffer, size_t bytes, bool* keep_alive) {
    bool force_close_connection = false;
    int status_code = 0;

    HttpRequest* req = handle->parse_http_request(buffer, bytes, &status_code);
    HttpResponse resp = {0};

    if (!req) {
        force_close_connection = true;
        resp = handle->handle_parse_http_request_error(req, status_code);
    } else {
        handle->find_route(handle->routes, handle->num_routes, req, &resp);
    }

    char* response = handle->build_http_response(&resp);
    if (!response) {
        force_close_connection = true;
        response = handle->handle_build_http_response_error(&resp);
    }

    *keep_alive = !force_close_connection && (req->keep_alive && !resp.close_connection);
    if (req) {
        if (req->body) {
            free(req->body);
        }
        free(req);
    }
    free_http_response(&resp);

    return response;
}