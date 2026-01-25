#include "router.h"
#include "http_errors.h"
#include <string.h>

// Given a set of routes, determine which route handler to use by comparing paths
void find_route(Route* routes, size_t num_routes, HttpRequest* req, HttpResponse* resp) {
    if (!req) {
        HttpResponse temp_resp = handle_404(req);
        *resp = temp_resp;
        return;
    }

    char clean_path[MAX_PATH_LEN];
    strcpy(clean_path, req->path);
    char* query_start = strchr(clean_path, '?');
    if (query_start) {
        *query_start = '\0';
    }

    for (size_t i = 0; i < num_routes; i++) {
        if (strcmp(clean_path, routes[i].path) == 0) {
            HttpResponse temp_resp = routes[i].handler(req);
            *resp = temp_resp;

            return;
        }
    }
    HttpResponse temp_resp = handle_404(req);
    *resp = temp_resp;
    return;
}