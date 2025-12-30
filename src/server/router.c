#include "router.h"
#include "http.h"
#include "http_errors.h"
#include <string.h>

// Given a set of routes, determine which route handler to use by comparing paths
void find_route(Route* routes, int num_routes, HttpRequest* req, HttpResponse* resp) {
    if (!req) {
        HttpResponse temp_resp = handle_404(req);
        *resp = temp_resp;
        return;
    }
    for (int i = 0; i < num_routes; i++) {
        if (strcmp(req->path, routes[i].path) == 0) {
            HttpResponse temp_resp = routes[i].handler(req);
            *resp = temp_resp;

            return;
        }
    }
    HttpResponse temp_resp = handle_404(req);
    *resp = temp_resp;
    return;
}